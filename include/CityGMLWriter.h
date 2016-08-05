#ifndef CITYGMLWRITER_H__
#define CITYGMLWRITER_H__

#include <QString>
#include <QXmlStreamWriter>
#include <QFile>
#include <QMessageBox>

#include <osg/Array>
#include <osg/Node>
#include <osg/MatrixTransform>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/StateSet>
#include <osg/BlendFunc>
#include <osg/BlendColor>
#include <osg/Material>
#include <osg/Texture2D>
#include <osg/TexGen>
#include <osg/TexMat>
#include <osg/Depth>
#include <osg/LightModel>
#include <osg/ValueObject>
#include <osg/Notify>

#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgDB/FileUtils>
#include <osgDB/FileNameUtils>

#include <osgUtil/SmoothingVisitor>

#include <citygml/citygml.h>
#include <citygml/envelope.h>
#include <citygml/citymodel.h>
#include <citygml/cityobject.h>
#include <citygml/geometry.h>
#include <citygml/polygon.h>
#include <citygml/material.h>
#include <citygml/texture.h>
#include <citygml/citygmllogger.h>

class CityGMLWriter {
    public :
        CityGMLWriter(QString fileName);
        void write(osg::Node* node);

    private :
        QString fileName;

};

#endif // CITYGMLWRITER_H__
