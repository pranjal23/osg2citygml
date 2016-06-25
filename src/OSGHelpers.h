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
                geometry->getTexCoordArrayList().clear();

                if (geometry->getColorArray()==0 || geometry->getColorArray()->getNumElements()==0)
                {
                    osg::Vec4Array* colours = new osg::Vec4Array(1);
                    osg::Vec4* col = getNewColor();
                    (*colours)[0].set(col->x(),col->y(),col->z(),col->w());
                    geometry->setColorArray(colours, osg::Array::BIND_PER_VERTEX);
                }
            }
        }
    }

    virtual void apply(osg::Node& node) { traverse(node); }

private:
    int x = 0;
    osg::Vec4* getNewColor(){
        x++;
        if(x % 2 == 0)
            return new osg::Vec4(1.0f,1.0f,1.0f,1.0f);
        else
            return new osg::Vec4(0.5f,0.5f,0.5f,0.5f);
    }

};

class ConvertToTrianglePrimitives : public osg::NodeVisitor
{
public:

    ConvertToTrianglePrimitives():osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) {}

    virtual void apply(osg::Geode& geode)
    {
        //convert to triangle strip and simplify
        osgUtil::TriStripVisitor triStripVisitor;
        triStripVisitor.apply(geode);

        osgUtil::Simplifier simplifier;
        simplifier.apply(geode);

        for(unsigned int i=0;i<geode.getNumDrawables();++i)
        {
            osg::Geometry* geometry = dynamic_cast<osg::Geometry*>(geode.getDrawable(i));

            //std::vector<osg::DrawElementsUInt>* removeSets = new std::vector<osg::DrawElementsUInt>;
            //std::vector<osg::DrawElementsUInt>* addSets = new std::vector<osg::DrawElementsUInt>;

            for (unsigned int ipr=0; ipr<geometry->getNumPrimitiveSets(); ipr++)
            {
                osg::PrimitiveSet* prset=geometry->getPrimitiveSet(ipr);
                osg::notify(osg::WARN) << "PRSET CLASS NAME: " << prset->getCompoundClassName();

                // for each primitive analyze its type and work accordingly...
                switch (prset->getMode())
                {
                    case osg::PrimitiveSet::TRIANGLES:
                    {
                        osg::notify(osg::WARN) << "In triangles - ";

                        unsigned int ja;
                        for (ja=0; ja<prset->getNumIndices(); ja++)
                        {
                            osg::DrawElementsUInt* primSet =
                                    new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);

                            //TODO add elements in the sets
                            {
                                unsigned int vertexId = prset->index(ja);
                                //primSet->push_back(vertexId);
                                 osg::notify(osg::WARN) << vertexId << ", ";
                            }

                            //addSets->push_back(*primSet);
                        }

                        osg::notify(osg::WARN) << std::endl;
                    }
                    break;

                    case osg::PrimitiveSet::TRIANGLE_STRIP:
                    {
                        osg::notify(osg::WARN) << "In triangle strip - ";

                        unsigned int ja;
                        for (ja=0; ja<prset->getNumIndices()-2; ja++)
                        {
                            osg::DrawElementsUInt* primSet =
                                    new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLE_STRIP, 0);

                            //TODO add elements in the sets
                            {
                                unsigned int vertexId = prset->index(ja);
                                //primSet->push_back(vertexId);
                                 osg::notify(osg::WARN) << vertexId << ", ";
                            }

                            //addSets->push_back(*primSet);
                        }

                        osg::notify(osg::WARN) << std::endl;
                    }
                    break;

                    //TODO: Handle other primitive types such as quads, quadstrips lines line loops...
                }

            }

        }
        //End of for-loop


    }

    virtual void apply(osg::Node& node) { traverse(node); }

};

#endif // OSGHELPERS

