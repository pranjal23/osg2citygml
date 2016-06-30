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
                unsigned int num = geometry->getNumPrimitiveSets();
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

                 qDebug() << "npc ...";

                for (unsigned int ipr=0; ipr<num; ipr++)
                {   
                    if(selectedPrimitives != nullptr && selectedPrimitives->size()>0)
                    {
                        std::multimap<unsigned int,SelectedTrianglePrimitive>::iterator it = selectedPrimitives->find(ipr);
                        for(;it!=selectedPrimitives->end();it++)
                        {
                            qDebug() << "In here ...";
                            SelectedTrianglePrimitive a =(*it).second;
                            if(a.drawable.get() == geometry->asDrawable())
                            {
                                x = 1;
                            }
                            else
                            {
                                x = 0;
                            }
                        }
                    }

                    osg::PrimitiveSet* prset=geometry->getPrimitiveSet(ipr);
                    (*colours)[prset->index(0)].set(col->x(),col->y(),col->z(),col->w());
                    col = getNewColor(x);
                    (*colours)[prset->index(1)].set(col->x(),col->y(),col->z(),col->w());
                    col = getNewColor(x);
                    (*colours)[prset->index(2)].set(col->x(),col->y(),col->z(),col->w());
                }
                geometry->setColorArray(colours, osg::Array::BIND_PER_PRIMITIVE_SET);
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
        qDebug() << "In selectIntersectedPrimitives...";

        osgUtil::LineSegmentIntersector::Intersection firstIntersection = *intersections.begin();

        if (firstIntersection.drawable == nullptr) {
            return false;
        }

        qDebug() << "Selected primitive...";
        SelectedTrianglePrimitive* stp = new SelectedTrianglePrimitive;
        stp->drawable = firstIntersection.drawable.get();
        stp->primitiveIndex = firstIntersection.primitiveIndex;

        selectedPrimitives->insert(std::pair<unsigned int,SelectedTrianglePrimitive>(stp->primitiveIndex,*stp));
        colorVistor->addSelectedPrimitivePtr(selectedPrimitives);
        addColor();

        return true;
    }

    int m_xMouseCoordAtLastPress;
    int m_yMouseCoordAtLastPress;
};

/*
class RayTriangleIntersectionHelper
{
public:

    RayTriangleIntersectionHelper() {}

    virtual void SUB(osg::Vec3 out, osg::Vec3 a, osg::Vec3 b){
        out.set(a.x() - b.x(), a.y() - b.y(), a.z() - b.z());
    }

    virtual void CROSS(osg::Vec3 out, osg::Vec3 a, osg::Vec3 b){

        out.set(a.y() * b.z() - a.z() * b.y(),
                a.z() * b.x() - a.x() * b.z(),
                a.x() * b.y() - a.y() * b.x());
    }

    virtual float DOT(osg::Vec3 a, osg::Vec3 b){

        return a.x() * b.x() + a.y() * b.y() + a.z() * b.z();
    }

    //Möller–Trumbore ray-triangle intersection algorithm
    const double EPSILON = -0.00001;
    virtual int triangle_intersection( const osg::Vec3   V1,  // Triangle vertices
                                       const osg::Vec3   V2,
                                       const osg::Vec3   V3,
                                       const osg::Vec3    O,  //Ray origin
                                       const osg::Vec3    D,  //Ray direction
                                       float* out )
    {

        if(true)
        {
            qDebug() << "Printing v1, v2, v3";
            printVertex(V1);
            printVertex(V2);
            printVertex(V3);

            qDebug() << "Printing origin, direction";
            printVertex(O);
            printVertex(D);
        }

        osg::Vec3 e1, e2;  //Edge1, Edge2
        osg::Vec3 P, Q, T;
        float det, inv_det, u, v;
        float t;

        //Find vectors for two edges sharing V1
        SUB(e1, V2, V1);
        SUB(e2, V3, V1);
        //Begin calculating determinant - also used to calculate u parameter
        CROSS(P, D, e2);
        //if determinant is near zero, ray lies in plane of triangle
        det = DOT(e1, P);
        //NOT CULLING
        if(det > -EPSILON && det < EPSILON) return 0;
        inv_det = 1.f / det;

        //calculate distance from V1 to ray origin
        SUB(T, O, V1);

        //Calculate u parameter and test bound
        u = DOT(T, P) * inv_det;
        //The intersection lies outside of the triangle
        if(u < 0.f || u > 1.f) return 0;

        //Prepare to test v parameter
        CROSS(Q, T, e1);

        //Calculate V parameter and test bound
        v = DOT(D, Q) * inv_det;
        //The intersection lies outside of the triangle
        if(v < 0.f || u + v  > 1.f) return 0;

        t = DOT(e2, Q) * inv_det;

        if(t > EPSILON) { //ray intersection
            *out = t;
            return 1;
        }

        // No hit, no win
        return 0;
    }

    virtual void cast(osg::Group& group, const osg::Vec3 origin, const osg::Vec3 direction )
    {
        qDebug() << " In Ray Cast ";
        int i;
        for (i = 0; i < group.getNumChildren(); i++)
        {
            osg::Geode* geode = (osg::Geode*)group.getChild(i);
            for(unsigned int i=0;i<geode->getNumDrawables();++i)
            {
                osg::Geometry* geometry = dynamic_cast<osg::Geometry*>(geode->getDrawable(i));

                const osg::Vec3Array* verts = dynamic_cast<osg::Vec3Array*>(geometry->getVertexArray());

                for (unsigned int ipr=0; ipr<geometry->getNumPrimitiveSets(); ipr++)
                {
                    osg::PrimitiveSet* prset=geometry->getPrimitiveSet(ipr);

                    //if(prset->get)
                    //{
                        const osg::Vec3 v1 = (* verts)[prset->index(0)];
                        const osg::Vec3 v2 = (* verts)[prset->index(1)];
                        const osg::Vec3 v3 = (* verts)[prset->index(2)];

                        float* out;
                        if(triangle_intersection(v1,v2,v3,origin,direction,out)==1)
                        {
                            qDebug() << " Triangle Selected ";
                        }
                        else
                        {
                             qDebug() << " TNS ";
                        }
                    //}

                }
            }
        }
    }

};
*/

#endif // RAYCASTHELPERS

