#ifndef RAYCASTHELPERS
#define RAYCASTHELPERS

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


class SelectedTrianglePrimitive
{
public:
    osg::ref_ptr<osg::Drawable> drawable;
    unsigned int primitiveIndex;
};

class AddEditColoursToGeometryVisitor : public osg::NodeVisitor
{
public:
    std::multimap<unsigned int,SelectedTrianglePrimitive>* selectedPrimitives;

    AddEditColoursToGeometryVisitor()
        :osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
    {
        selectedPrimitives = nullptr;
    }

    void addSelectedPrimitivePtr(std::multimap<unsigned int,SelectedTrianglePrimitive>* sp)
    {
        selectedPrimitives = sp;
    }

    virtual void apply(osg::Geode& geode)
    {
        qDebug() << "In add color ...";
        for(unsigned int i=0;i<geode.getNumDrawables();i++)
        {
            osg::Geometry* geometry = dynamic_cast<osg::Geometry*>(geode.getDrawable(i));
            if (geometry && geometry->getNumPrimitiveSets()>0)
            {
                //unsigned int num = geometry->getNumPrimitiveSets();
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


                if(selectedPrimitives != nullptr && selectedPrimitives->size()>0)
                {
                    for(std::multimap<unsigned int,SelectedTrianglePrimitive>::iterator it = selectedPrimitives->begin();it!=selectedPrimitives->end();it++)
                    {
                        SelectedTrianglePrimitive a =(*it).second;
                        //qDebug() << a.drawable.get() << " == " << geometry->asDrawable();

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
    std::multimap<unsigned int,SelectedTrianglePrimitive>* selectedPrimitives;
    osg::ref_ptr<osg::Group> editableModelGroup;
    AddEditColoursToGeometryVisitor* colorVistor
    = new AddEditColoursToGeometryVisitor();

    PickingHandler(std::multimap<unsigned int,SelectedTrianglePrimitive>* sp,
                   osg::ref_ptr<osg::Group> eg) {
        m_xMouseCoordAtLastPress = -1;
        m_yMouseCoordAtLastPress = -1;
        selectedPrimitives = sp;
        editableModelGroup = eg;
        addColor();
    }

    // EventHandler interface
    virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, osg::Object*, osg::NodeVisitor*) {
        if (ea.getEventType() == osgGA::GUIEventAdapter::RELEASE && ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON) {

            if (m_xMouseCoordAtLastPress != ea.getX() || m_yMouseCoordAtLastPress != ea.getY()) {
                return false;
            }

            return selectIntersectedPrimitives(ea, aa);


        } else if (ea.getEventType() == osgGA::GUIEventAdapter::PUSH && ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON) {
            m_xMouseCoordAtLastPress = ea.getX();
            m_yMouseCoordAtLastPress = ea.getY();
        }

        return false;
    }

private:

    void addColor(){

        if(editableModelGroup == nullptr)
            return;

        int i;
        for (i = 0; i < editableModelGroup.get()->getNumChildren(); i++)
        {
            osg::Geode* geode = (osg::Geode*)editableModelGroup.get()->getChild(i);
            colorVistor->apply(*geode);
        }

    }

    bool selectIntersectedPrimitives(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) {

        osgUtil::LineSegmentIntersector::Intersections intersections;
        if (!aa.computeIntersections(ea, intersections)) {
            return false;
        }
        //qDebug() << "In selectIntersectedPrimitives...";

        osgUtil::LineSegmentIntersector::Intersection firstIntersection = *intersections.begin();

        if (firstIntersection.drawable == nullptr) {
            return false;
        }

        SelectedTrianglePrimitive* stp = new SelectedTrianglePrimitive;
        stp->drawable = firstIntersection.drawable.get();
        stp->primitiveIndex = firstIntersection.primitiveIndex;

        qDebug() << "Selected primitive-> " << stp->primitiveIndex
                 << "Selected drawable-> " << stp->drawable.get();

        selectedPrimitives->insert(std::pair<unsigned int,SelectedTrianglePrimitive>(stp->primitiveIndex,*stp));
        colorVistor->addSelectedPrimitivePtr(selectedPrimitives);
        addColor();

        return true;
    }

    int m_xMouseCoordAtLastPress;
    int m_yMouseCoordAtLastPress;
};

#endif // RAYCASTHELPERS

