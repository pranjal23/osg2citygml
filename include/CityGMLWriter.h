#pragma once
#ifndef CITYGMLWRITER_H__
#define CITYGMLWRITER_H__

#include <QString>
#include <QFileInfo>
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

#include "OSGWidget.h"

class CityGMLWriter {
    public :
        CityGMLWriter(QString fileName, OSGWidget* osgwidget);
        void write(osg::Group* group);

    private :
        QString fileName;
        OSGWidget* osgwidget;
        void writeDescription(QXmlStreamWriter& xmlWriter);
        void writeName(QXmlStreamWriter& xmlWriter);
        void writeNameSpaces(QXmlStreamWriter& xmlWriter);
        void writeCityObjectMember(osg::Group* group, QXmlStreamWriter& xmlWriter);
        void writeCityObjectGroup(osg::Group* group, QXmlStreamWriter& xmlWriter);
        void writeBuildingGeometry(osg::Group* group , QXmlStreamWriter& xmlWriter, QString name_space, QString element_name);

};

#endif // CITYGMLWRITER_H__
