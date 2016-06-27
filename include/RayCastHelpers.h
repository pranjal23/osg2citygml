#ifndef RAYCASTHELPERS
#define RAYCASTHELPERS

#include <QDebug>

#include <osg/Group>
#include <osg/Vec3>

#include "OSGHelpers.h"


class SelectedTrianglePrimitive
{
public:
    unsigned int drawableId;
    unsigned int primitiveId;
};

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

#endif // RAYCASTHELPERS

