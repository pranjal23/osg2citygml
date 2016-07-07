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
    unsigned int primitiveIndex = -1000;
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

    AddEditColoursToGeometryVisitor* colorVistor = new AddEditColoursToGeometryVisitor();

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

    void setLocationBasedSegmentation(bool state)
    {
        locBased = state;
    }

    void setNormalsBasedSegmentation(bool state)
    {
        normalsBased = state;
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

    bool compareNormals(osg::Vec3f m, osg::Vec3f n)
    {
        //qDebug() << "Comparing Normal: x->" << m.x() << ", y->" << m.y() << ", z-> " << m.z();
        //qDebug() << "to: x->" << n.x() << ", y->" << n.y() << ", z-> " << n.z();

        if(m.x() == n.x() && m.y() == n.y() && m.z() == n.z())
            return true;

        return false;
    }

    void selectBySegmentation(SelectedTrianglePrimitive* stp, bool locBased, bool normalsBased)
    {
        osg::Geometry* geometry = stp->drawable.get()->asGeometry();
        unsigned int primIndx = stp->primitiveIndex;

        if(normalsBased)
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
                        //qDebug() << "Returned true:";
                        selectPrimitive(stp->drawable.get(),i,false, false);
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
        //qDebug() << "In selectIntersectedPrimitives...";

        osgUtil::LineSegmentIntersector::Intersection firstIntersection = *intersections.begin();

        if (firstIntersection.drawable == nullptr) {
            return false;
        }

        selectPrimitive(firstIntersection.drawable.get(),
                        firstIntersection.primitiveIndex,
                        true,
                        true);


        return true;
    }

    bool selectPrimitive(osg::Drawable* drawable,
                         unsigned int index,
                         bool propagate,
                         bool onUserAction )
    {
        SelectedTrianglePrimitive* stp = new SelectedTrianglePrimitive;
        stp->drawable = drawable;
        stp->primitiveIndex = index;

        //Find if exists selected map then then remove
        bool unselected = true;
        if(selectedPrimitives != nullptr && selectedPrimitives->size()>0)
        {
            std::multimap<unsigned int,SelectedTrianglePrimitive>::iterator itS;
            for(std::multimap<unsigned int,SelectedTrianglePrimitive>::iterator it = selectedPrimitives->begin();it!=selectedPrimitives->end();it++)
            {
                SelectedTrianglePrimitive a =(*it).second;
                if(a.drawable.get() == stp->drawable
                        && a.primitiveIndex == stp->primitiveIndex)
                {
                    itS = it;
                    unselected = false;
                    break;
                }
            }

            if(!unselected)
            {
                selectedPrimitives->erase(itS);
            }
        }

        //Insert into selected primitives list if newly selected
        if(unselected)
        {
            selectedPrimitives->insert(std::pair<unsigned int,SelectedTrianglePrimitive>(stp->primitiveIndex,*stp));

            if(propagate)
                selectBySegmentation(stp,locBased,normalsBased);
        }

        colorVistor->addSelectedPrimitivePtr(selectedPrimitives);
        addColor();
    }

    int m_xMouseCoordAtLastPress;
    int m_yMouseCoordAtLastPress;

    bool locBased;
    bool normalsBased;
};

#endif // RAYCASTHELPERS

