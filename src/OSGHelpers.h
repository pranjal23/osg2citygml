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
    float x = 0.0f;
    osg::Vec4* getNewColor(){
        float r = 0.0f;
        float g = 0.0f;
        float b = 1.0f;
        if (x >= 0.0f && x < 0.2f) {
                x = x / 0.2f;
                r = 0.0f;
                g = x;
                b = 1.0f;
        } else if (x >= 0.2f && x < 0.4f) {
                x = (x - 0.2f) / 0.2f;
                r = 0.0f;
                g = 1.0f;
                b = 1.0f - x;
        } else if (x >= 0.4f && x < 0.6f) {
                x = (x - 0.4f) / 0.2f;
                r = x;
                g = 1.0f;
                b = 0.0f;
        } else if (x >= 0.6f && x < 0.8f) {
                x = (x - 0.6f) / 0.2f;
                r = 1.0f;
                g = 1.0f - x;
                b = 0.0f;
        } else if (x >= 0.8f && x <= 1.0f) {
                x = (x - 0.8f) / 0.2f;
                r = 1.0f;
                g = 0.0f;
                b = x;
        }
        return new osg::Vec4(r,g,b,0.5f);
    }

};

#endif // OSGHELPERS

