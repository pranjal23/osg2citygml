#ifndef RAYCASTHELPERS_H__
#define RAYCASTHELPERS_H__

#include <QDebug>

#include <osg/Group>
#include <osg/Vec3>

#include <osgViewer/Viewer>
#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osg/Node>

#include <osgGA/SphericalManipulator>
#include <osgGA/StateSetManipulator>
#include <osgGA/GUIEventHandler>
#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIActionAdapter>
#include <osgViewer/ViewerEventHandlers>

#include <osgUtil/LineSegmentIntersector>
#include <osgUtil/Optimizer>

#include <iostream>
#include <map>
#include <utility>

#include "OSGHelpers.h"
#include "OSGWidget.h"

class AddEditColoursToGeometryVisitor : public osg::NodeVisitor
{
public:
    AddEditColoursToGeometryVisitor()
        :osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
    {}

    void apply(osg::Geode& geode,
               std::multimap<unsigned int,PrimitiveNode>& selectedPrimitives)
    {
        //qDebug() << "In add color ...";
        for(unsigned int i=0;i<geode.getNumDrawables();i++)
        {
            osg::Geometry* geometry = dynamic_cast<osg::Geometry*>(geode.getDrawable(i));
            if (geometry && geometry->getNumPrimitiveSets()>0)
            {
                //Remove Material and Colormark
                geometry->getOrCreateStateSet()->removeAttribute(osg::StateAttribute::MATERIAL);
                geometry->getOrCreateStateSet()->removeAttribute(osg::StateAttribute::COLORMASK);

                //Set all the vertex colors to editable color
                osg::Vec4Array* colours =
                        new osg::Vec4Array(geometry->getNumPrimitiveSets());
                int x = 0;
                osg::Vec4* col = getNewColor(x);
                for(unsigned int ci=0;ci<geometry->getNumPrimitiveSets();ci++)
                {
                    (*colours)[ci].set(col->x(),col->y(),col->z(),col->w());
                }

                if(selectedPrimitives.size()>0)
                {
                    for(std::multimap<unsigned int,PrimitiveNode>::iterator it = selectedPrimitives.begin();it!=selectedPrimitives.end();it++)
                    {
                        PrimitiveNode a =(*it).second;
                        if(a.drawable.get() == geometry->asDrawable())
                        {
                            x = 1;
                            col = getNewColor(x);
                            (*colours)[a.primitiveIndex].set(col->x(),col->y(),col->z(),col->w());
                        }
                    }
                }
                geometry->setColorArray(colours, osg::Array::BIND_PER_PRIMITIVE_SET);
            }
        }
    }

    virtual void apply(osg::Node& node) { traverse(node); }

private:
    osg::Vec4* getNewColor(int x){
        if(x == 0)
            return new osg::Vec4(1.0f,0.5f,0.5f,1.0f);
        else
            return new osg::Vec4(0.5f,1.0f,1.0f,1.0f);
    }

};

class PickingHandler : public osgGA::GUIEventHandler {
public:
    OSGWidget* osgwidget;
    std::multimap<unsigned int,PrimitiveNode>* selectedPrimitives;

    AddEditColoursToGeometryVisitor* colorVistor = new AddEditColoursToGeometryVisitor();

    PickingHandler(OSGWidget* widget)
    {
        osgwidget = widget;
        m_xMouseCoordAtLastPress = -1;
        m_yMouseCoordAtLastPress = -1;
        selectedPrimitives = new std::multimap<unsigned int,PrimitiveNode>();
        addColor();
    }

    // EventHandler interface
    virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, osg::Object*, osg::NodeVisitor*)
    {
        if(!osgwidget->renderEditableMode)
            return false;

        if (ea.getEventType() == osgGA::GUIEventAdapter::RELEASE && ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
        {

            if (m_xMouseCoordAtLastPress != ea.getX() || m_yMouseCoordAtLastPress != ea.getY())
            {
                return false;
            }

            return selectIntersectedPrimitives(ea, aa);

        }
        else if (ea.getEventType() == osgGA::GUIEventAdapter::PUSH && ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
        {
            m_xMouseCoordAtLastPress = ea.getX();
            m_yMouseCoordAtLastPress = ea.getY();
        }


        if(ea.getEventType() == osgGA::GUIEventAdapter::KEYDOWN)
        {
            switch(ea.getKey())
            {
            case 'c':
                clearSelectedList();
                return true;
                break;
            default:
                return false;
            }
        }

        return false;
    }

    void addToSelectedPrimitiveList(QList<PrimitiveNode>& prim_list)
    {
        for(int i=0; i<prim_list.size(); i++)
        {
            selectedPrimitives->insert(
                        std::pair<unsigned int,PrimitiveNode>(prim_list.at(i).primitiveIndex,prim_list.at(i)));
        }
        addColor();
    }

    void clearSelectedList()
    {
        selectedPrimitives->clear();
        addColor();
    }

private:

    void addColor()
    {

        if(osgwidget->getEditableModelGroup() == nullptr)
            return;

        int i;
        for (i = 0; i < osgwidget->getEditableModelGroup().get()->getNumChildren(); i++)
        {
            osg::Geode* geode = (osg::Geode*)osgwidget->getEditableModelGroup().get()->getChild(i);
            colorVistor->apply(*geode,*selectedPrimitives);
        }

    }

    bool compareNormals(osg::Vec3f m, osg::Vec3f n)
    {
        double weight = osgwidget->getNormalsDistance();
        if(std::abs(m.x() - n.x()) <= weight
                && std::abs(m.y() - n.y()) <= weight
                && std::abs(m.z() - n.z()) <= weight)
            return true;

        return false;
    }

    void propagateBySegmentation(PrimitiveNode& stp)
    {
        if(osgwidget->getNormalsBasedSegmentation() && !osgwidget->getLocationBasedSegmentation())
        {
            osg::Vec3f selectedNormal = *(stp.faceNormal);

            QList<PrimitiveNode> list = osgwidget->getAllPolygonNodes();
            for (unsigned int i = 0; i < list.size(); i++)
            {
                if(i == stp.primitiveIndex && list.at(i).drawable == stp.drawable)
                    continue;

                osg::Vec3f currNormal = *(list.at(i).faceNormal);

                if(compareNormals(selectedNormal,currNormal))
                {
                    selectDeselectPrimitive(list.at(i).drawable,list.at(i).primitiveIndex,false);
                }
            }
        }
        else if(osgwidget->getLocationBasedSegmentation())
        {
            for(unsigned int i=0; i < stp.links->size(); i++)
            {
                selectDeselectPrimitive(stp.links->at(i).drawable,stp.links->at(i).primitiveIndex,false);
            }
        }
    }

    bool selectIntersectedPrimitives(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) {

        osgUtil::LineSegmentIntersector::Intersections intersections;
        if (!aa.computeIntersections(ea, intersections)) {
            return false;
        }

        osgUtil::LineSegmentIntersector::Intersection firstIntersection = *intersections.begin();


        if (firstIntersection.drawable == nullptr) {
            return false;
        }

        selectDeselectPrimitive(firstIntersection.drawable.get(),
                                firstIntersection.primitiveIndex,
                                true);
        addColor();

        return true;
    }

    void selectDeselectPrimitive(osg::Drawable* drawable,
                                 unsigned int index,
                                 bool propagate)
    {

        PrimitiveNode stp = osgwidget->getPolygonNode(drawable,index);

        std::multimap<unsigned int,PrimitiveNode>::iterator itS;
        if(osgwidget->getEditableModelGroup() != nullptr)
        {
            if(!osgwidget->selectMode)
            {
                bool selected = false;
                std::pair <std::multimap<unsigned int,PrimitiveNode>::iterator, std::multimap<unsigned int,PrimitiveNode>::iterator> ret;
                ret = selectedPrimitives->equal_range(stp.primitiveIndex);
                for (std::multimap<unsigned int,PrimitiveNode>::iterator it=ret.first; it!=ret.second; ++it)
                {
                    PrimitiveNode a = it->second;
                    if(a.drawable == stp.drawable)
                    {
                        itS = it;
                        selected = true;
                        break;
                    }
                }

                if(selected)
                {
                    selectedPrimitives->erase(itS);
                    if(propagate)
                        propagateBySegmentation(stp);
                }
            }


            //Insert into selected primitives list if newly selected
            if(osgwidget->selectMode)
            {
                selectedPrimitives->insert(std::pair<unsigned int,PrimitiveNode>(stp.primitiveIndex,stp));
                if(propagate)
                    propagateBySegmentation(stp);
            }

        }
    }

    int m_xMouseCoordAtLastPress;
    int m_yMouseCoordAtLastPress;
};

#endif // RAYCASTHELPERS_H__

