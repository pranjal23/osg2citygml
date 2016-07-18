#ifndef CITYGMLWRITER
#define CITYGMLWRITER

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

#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgDB/FileUtils>
#include <osgDB/FileNameUtils>

#include <osgUtil/SmoothingVisitor>

#include <osg/Notify>

class CityGMLWriter {
    public :
        CityGMLWriter(QString fileName);
        void write(osg::Node* node);

    private :
        QString fileName;

};

#endif // CITYGMLWRITER
