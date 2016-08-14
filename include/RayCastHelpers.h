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
               std::multimap<unsigned int,TrianglePrimitive>& selectedPrimitives)
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

                osg::Vec4Array* colours =
                        new osg::Vec4Array(geometry->getVertexArray()->getNumElements());
                int x = 0;
                osg::Vec4* col = getNewColor(x);
                for(unsigned int ci=0;ci<colours->size();ci++)
                {
                    (*colours)[ci].set(col->x(),col->y(),col->z(),col->w());
                }

                //qDebug() << "npc ...";


                if(selectedPrimitives.size()>0)
                {
                    for(std::multimap<unsigned int,TrianglePrimitive>::iterator it = selectedPrimitives.begin();it!=selectedPrimitives.end();it++)
                    {
                        TrianglePrimitive a =(*it).second;

                        if(a.drawable.get() == geometry->asDrawable())
                        {
                            x = 1;
                            unsigned int ipr = (*it).first;
                            osg::PrimitiveSet* prset=geometry->getPrimitiveSet(ipr);

                            //qDebug() << "Colored primitive-> " << ipr;
                            //printPrimSet(prset);

                            col = getNewColor(x);
                            (*colours)[prset->index(0)].set(col->x(),col->y(),col->z(),col->w());
                            col = getNewColor(x);
                            (*colours)[prset->index(1)].set(col->x(),col->y(),col->z(),col->w());
                            col = getNewColor(x);
                            (*colours)[prset->index(2)].set(col->x(),col->y(),col->z(),col->w());
                        }
                    }
                }


                geometry->setColorArray(colours, osg::Array::BIND_PER_VERTEX);
            }
        }
    }

    virtual void apply(osg::Node& node) { traverse(node); }

private:
    osg::Vec4* getNewColor(int x){
        if(x == 0)
            return new osg::Vec4(1.0f,0.5f,1.0f,1.0f);
        else
            return new osg::Vec4(0.5f,1.0f,1.0f,1.0f);
    }

};

class PickingHandler : public osgGA::GUIEventHandler {
public:
    OSGWidget* osgwidget;
    std::multimap<unsigned int,TrianglePrimitive>* selectedPrimitives;

    AddEditColoursToGeometryVisitor* colorVistor = new AddEditColoursToGeometryVisitor();

    PickingHandler(OSGWidget* widget)
    {
        osgwidget = widget;
        m_xMouseCoordAtLastPress = -1;
        m_yMouseCoordAtLastPress = -1;
        selectedPrimitives = new std::multimap<unsigned int,TrianglePrimitive>();
        addColor();
    }

    // EventHandler interface
    virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, osg::Object*, osg::NodeVisitor*)
    {
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
                qDebug() << " c key pressed";
                clearSelectedList();
                return true;
                break;
            default:
                return false;
            }
        }

        return false;
    }

    void addToSelectedPrimitiveList(QList<TrianglePrimitive>* prim_list)
    {
        for(int i=0; i<prim_list->size(); i++)
        {
            selectedPrimitives->insert(
                        std::pair<unsigned int,TrianglePrimitive>(prim_list->at(i).primitiveIndex,prim_list->at(i)));
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
        if(m.x() - n.x() <= weight
                && m.y() - n.y() <= weight
                && m.z() - n.z() <= weight)
            return true;

        return false;
    }

    void propagateBySegmentation(TrianglePrimitive& stp)
    {
        osg::Geometry* geometry = stp.drawable.get()->asGeometry();
        unsigned int primIndx = stp.primitiveIndex;

        if(osgwidget->getNormalsBasedSegmentation())
        {
            //Get the Normal for the triangle primitive
            UserData* userData = dynamic_cast<UserData*>(geometry->getUserData());
            osg::Vec3Array* faceNormals
                    = dynamic_cast<osg::Vec3Array*>(userData->faceNormals.get());

            osg::Vec3f selectedNormal = (*faceNormals)[primIndx];
            if(faceNormals != nullptr)
            {
                for (unsigned int i = 0; i < faceNormals->size(); i++)
                {
                    if(i == primIndx)
                        continue;

                    osg::Vec3f currNormal = (*faceNormals)[i];

                    if(compareNormals(selectedNormal,currNormal))
                    {
                        selectDeselectPrimitive(stp.drawable.get(),i,false);
                    }
                }
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


        return true;
    }

    void selectDeselectPrimitive(osg::Drawable* drawable,
                         unsigned int index,
                         bool propagate)
    {

        TrianglePrimitive stp = osgwidget->getPrimitive(drawable,index);

        std::multimap<unsigned int,TrianglePrimitive>::iterator itS;
        if(osgwidget->getEditableModelGroup() != nullptr && selectedPrimitives->size()>0)
        {
            std::pair <std::multimap<unsigned int,TrianglePrimitive>::iterator, std::multimap<unsigned int,TrianglePrimitive>::iterator> ret;
            ret = selectedPrimitives->equal_range(stp.primitiveIndex);
            for (std::multimap<unsigned int,TrianglePrimitive>::iterator it=ret.first; it!=ret.second; ++it)
            {
                TrianglePrimitive a = it->second;
                if(a.drawable == stp.drawable)
                {
                    itS = it;
                    break;
                }
            }

           if(!osgwidget->selectMode)
            {
                selectedPrimitives->erase(itS);
                if(propagate)
                    propagateBySegmentation(stp);
            }

        }

        //Insert into selected primitives list if newly selected
        if(osgwidget->selectMode)
        {
            selectedPrimitives->insert(std::pair<unsigned int,TrianglePrimitive>(stp.primitiveIndex,stp));
            if(propagate)
                propagateBySegmentation(stp);
        }

        addColor();
    }

    int m_xMouseCoordAtLastPress;
    int m_yMouseCoordAtLastPress;
};

#endif // RAYCASTHELPERS_H__

