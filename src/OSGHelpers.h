#ifndef OSGHELPERS
#define OSGHELPERS
#include <stdio.h>

#include <osg/ArgumentParser>
#include <osg/ApplicationUsage>
#include <osg/Group>
#include <osg/Notify>
#include <osg/Vec3>
#include <osg/ProxyNode>
#include <osg/Geometry>
#include <osg/Texture2D>
#include <osg/Texture3D>
#include <osg/BlendFunc>
#include <osg/Timer>

#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgDB/FileNameUtils>
#include <osgDB/ReaderWriter>
#include <osgDB/PluginQuery>

#include <osgUtil/Optimizer>
#include <osgUtil/Simplifier>
#include <osgUtil/TriStripVisitor>
#include <osgUtil/SmoothingVisitor>

#include <osgViewer/GraphicsWindow>
#include <osgViewer/Version>

#include <iostream>

void printPrimSet(osg::PrimitiveSet*prset)
{
    unsigned int ic;

    osg::notify(osg::WARN) << "Prim set type - " << prset->getMode() << ", Vertex Ids: ";

    for (ic=0; ic < prset->getNumIndices(); ic++)
    {
        unsigned int vertexId = prset->index(ic);
        osg::notify(osg::WARN) << vertexId << ", ";
    }

    osg::notify(osg::WARN) << " | ";
}

void printVertex(unsigned int vertexId, const osg::Vec3Array *verts)
{
    osg::notify(osg::DEBUG_FP) << "Vertex Id: "<< vertexId << ", coordinates: " <<
                                  (* verts)[vertexId].x() << "," <<
                                  (* verts)[vertexId].y() << "," <<
                                  (* verts)[vertexId].z() << std::endl;
}

class AddEditColoursToGeometryVisitor : public osg::NodeVisitor
{
public:

    AddEditColoursToGeometryVisitor():osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) {}

    virtual void apply(osg::Geode& geode)
    {
        for(unsigned int i=0;i<geode.getNumDrawables();++i)
        {
            osg::Geometry* geometry = dynamic_cast<osg::Geometry*>(geode.getDrawable(i));
            if (geometry)
            {
                geometry->getOrCreateStateSet()->removeAttribute(osg::StateAttribute::MATERIAL);
                geometry->getOrCreateStateSet()->removeAttribute(osg::StateAttribute::COLORMASK);

                osg::Vec4Array* colours = new osg::Vec4Array(3);
                osg::Vec4* col = getNewColor(0);
                (*colours)[0].set(col->x(),col->y(),col->z(),col->w());
                col = getNewColor(1);
                (*colours)[1].set(col->x(),col->y(),col->z(),col->w());
                col = getNewColor(2);
                (*colours)[2].set(col->x(),col->y(),col->z(),col->w());
                geometry->setColorArray(colours, osg::Array::BIND_OVERALL);
            }
        }
    }

    virtual void apply(osg::Node& node) { traverse(node); }

private:
    osg::Vec4* getNewColor(int x){
        if(x == 0)
            return new osg::Vec4(0.5f,1.0f,1.0f,1.0f);
        else if(x == 1)
            return new osg::Vec4(1.0f,0.5f,1.0f,1.0f);
        else if(x == 2)
            return new osg::Vec4(1.0f,1.0f,0.5f,1.0f);
    }

};

class TriangleIndexes
{
public:
    unsigned int vertexId1;
    unsigned int vertexId2;
    unsigned int vertexId3;
};


class ConvertToTrianglePrimitives : public osg::NodeVisitor
{
public:

    ConvertToTrianglePrimitives():osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) {}

    bool verbose = false;
    bool printIndexes = false;
    virtual void apply(osg::Geode& geode)
    {

        //convert to triangle strip
        osgUtil::TriStripVisitor triStripVisitor;
        triStripVisitor.apply(geode);
        triStripVisitor.stripify();

        for(unsigned int i=0;i<geode.getNumDrawables();++i)
        {
            osg::Geometry* geometry = dynamic_cast<osg::Geometry*>(geode.getDrawable(i));

            std::vector<TriangleIndexes> addPrimSetIndexes;

            for (unsigned int ipr=0; ipr<geometry->getNumPrimitiveSets(); ipr++)
            {
                osg::PrimitiveSet* prset=geometry->getPrimitiveSet(ipr);

                //osg::notify(osg::WARN) << "PRSET CLASS NAME: " << prset->getCompoundClassName();

                //osg::notify(osg::WARN) << "---- PRINTING BEFORE PARSING OF PRIMITIVES ----" << std::endl;
                // for each primitive analyze its type and work accordingly...
                switch (prset->getMode())
                {
                    case osg::PrimitiveSet::TRIANGLES:
                    {

                        osg::notify(osg::WARN) << "In triangles - ";

                        unsigned int ja;
                        for (ja=0; ja<=prset->getNumIndices()-3; )
                        {
                            TriangleIndexes ti;

                            ti.vertexId1 = prset->index(ja);
                            ti.vertexId2 = prset->index(ja+1);
                            ti.vertexId3 = prset->index(ja+2);

                            if(verbose && printIndexes)
                            {
                                osg::notify(osg::WARN) << ti.vertexId1 << ", "
                                                       << ti.vertexId2 << ", "
                                                       << ti.vertexId3 << std::endl;
                            }

                            addPrimSetIndexes.push_back(ti);
                            ja=ja+3;
                        }
                    }
                    break;

                    case osg::PrimitiveSet::TRIANGLE_STRIP:
                    {
                        osg::notify(osg::WARN) << "In triangle strip - ";

                        unsigned int ja;
                        for (ja=0; ja<prset->getNumIndices()-2; ja++)
                        {
                            TriangleIndexes ti;

                            if(ja % 2 == 0)
                            {
                                ti.vertexId1 = prset->index(ja);
                                ti.vertexId2 = prset->index(ja+1);
                                ti.vertexId3 = prset->index(ja+2);
                            }
                            else
                            {
                                ti.vertexId1 = prset->index(ja);
                                ti.vertexId2 = prset->index(ja+2);
                                ti.vertexId3 = prset->index(ja+1);
                            }

                            if(verbose && printIndexes)
                            {
                                osg::notify(osg::WARN) << ti.vertexId1 << ", "
                                                       << ti.vertexId2 << ", "
                                                       << ti.vertexId3 << std::endl;
                            }

                            addPrimSetIndexes.push_back(ti);
                        }
                    }
                    break;

                    //TODO: Check and Handle other primitive types such as lines line loops etc
                    //that is not converted into triangle_strip by TriStripVisitor...
                }

            }

            //osg::notify(osg::WARN) << "---- PRINTING BEFORE REMOVAL OF PRIMITIVES ----" << std::endl;
            geometry->removePrimitiveSet(0,geometry->getNumPrimitiveSets());

            for (unsigned int prn=0; prn<addPrimSetIndexes.size(); prn++)
            {
                osg::DrawElementsUShort* primSet =
                        new osg::DrawElementsUShort(osg::PrimitiveSet::TRIANGLES, 0);
                TriangleIndexes ti = addPrimSetIndexes.at(prn);
                primSet->push_back(ti.vertexId1);
                primSet->push_back(ti.vertexId2);
                primSet->push_back(ti.vertexId3);
                geometry->addPrimitiveSet(primSet);
            }

            if(verbose)
            {
                osg::notify(osg::WARN) << "---- PRINTING PRIMITIVES AFTER CONVERSION ----" << std::endl;

                for (unsigned int ipr=0; ipr<geometry->getNumPrimitiveSets(); ipr++)
                {
                    osg::PrimitiveSet* prset=geometry->getPrimitiveSet(ipr);
                    printPrimSet(prset);
                }
                osg::notify(osg::WARN) << "---- AFTER PRINTING PRIMITIVES ----" << std::endl;
            }

        }

        //Simplify before returning...
        osgUtil::Simplifier simplifier;
        simplifier.apply(geode);

    }

    virtual void apply(osg::Node& node) { traverse(node); }

    void setVerbose(bool v){
        verbose = v;
    }

};

#endif // OSGHELPERS

