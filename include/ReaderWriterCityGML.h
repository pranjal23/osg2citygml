///////////////////////////////////////////////////////////////////////////////
// OSG plugin for reading OGC CityGML v0.3 - v1.0 format using libcitygml
// http://code.google.com/p/libcitygml
// Copyright(c) 2010 Joachim Pouderoux, BRGM
//////////////////////////////////////////////////////////////////////////

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

#include <osgText/Font>
#include <osgText/Text>

#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgDB/FileUtils>
#include <osgDB/FileNameUtils>

#include <osgUtil/SmoothingVisitor>

#include <osg/Notify>

#include <citygml/citygml.h>
#include <citygml/envelope.h>
#include <citygml/citymodel.h>
#include <citygml/cityobject.h>
#include <citygml/geometry.h>
#include <citygml/polygon.h>
#include <citygml/material.h>
#include <citygml/texture.h>
#include <citygml/citygmllogger.h>

#include <algorithm>
#include <cctype>

class CityGMLOSGPluginLogger : public citygml::CityGMLLogger {
public:

    virtual void log(LOGLEVEL level, const std::string& message, const char* file, int line) const
    {
        std::ostream& stream = getLogStreamFor(level);

        if (file) {
            stream << " [" << file;
            if (line > -1) {
                stream << ":" << line;
            }
            stream << "]";
        }

        stream << " " << message << std::endl;
    }

private:
    std::ostream& getLogStreamFor(LOGLEVEL level) const {
        switch(level) {
        case LOGLEVEL::LL_DEBUG:
            return osg::notify(osg::DEBUG_INFO);
        case LOGLEVEL::LL_WARNING:
            return osg::notify(osg::WARN);
        case LOGLEVEL::LL_TRACE:
            return osg::notify(osg::DEBUG_FP);
        case LOGLEVEL::LL_ERROR:
            return osg::notify(osg::FATAL);
        case LOGLEVEL::LL_INFO:
            return osg::notify(osg::INFO);
        default:
            return osg::notify(osg::INFO);
        }
    }
};

class CityGMLSettings
{
public:
    CityGMLSettings( void )
        : _printNames(false)
        , _useMaxLODOnly(false)
        , _storeGeomIDs(false)
        , _theme("")
    {}

    void parseOptions( const osgDB::ReaderWriter::Options* options)
    {
        if ( !options ) return;
        std::istringstream iss( options->getOptionString() );
        std::string currentOption;
        while ( iss >> currentOption )
        {
            std::transform( currentOption.begin(), currentOption.end(), currentOption.begin(), ::tolower );
            if ( currentOption == "names" ) _printNames = true;
            else if ( currentOption == "mask" ) iss >> _params.objectsMask;
            else if ( currentOption == "minlod" ) iss >> _params.minLOD;
            else if ( currentOption == "maxlod" ) iss >> _params.maxLOD;
            else if ( currentOption == "optimize" ) _params.optimize = true;
            else if ( currentOption == "pruneemptyobjects" ) _params.pruneEmptyObjects = true;
            else if ( currentOption == "usemaxlodonly" ) _useMaxLODOnly = true;
            else if ( currentOption == "usetheme" ) iss >> _theme;
            else if ( currentOption == "storegeomids" ) _storeGeomIDs = true;
        }
    }

public:
    citygml::ParserParams _params;
    bool _printNames;
    bool _useMaxLODOnly;
    bool _storeGeomIDs;
    std::map< std::string, osg::Texture2D* > _textureMap;
    std::string _theme;
};

class ReaderWriterCityGML : public osgDB::ReaderWriter
{
public:
    ReaderWriterCityGML( void )
    {
        supportsExtension( "citygml", "CityGML format" );
        supportsExtension( "gml", "CityGML format" );

        supportsOption( "names", "Add the name of the city objects on top of them" );
        supportsOption( "mask", "Set the objects mask" );
        supportsOption( "minLOD", "Minimum LOD level to fetch" );
        supportsOption( "maxLOD", "Maximum LOD level to fetch" );
        supportsOption( "optimize", "Optimize the geometries & polygons of the CityGML model to reduce the number of instanced objects" );
        supportsOption( "pruneEmptyObjects", "Prune empty objects (ie. without -supported- geometry)" );
        supportsOption( "destSRS", "Transform geometry to given reference system" );
        supportsOption( "useMaxLODonly", "Use the highest available LOD for geometry of one object" );
        supportsOption( "appearanceTheme", "Name of the appearance theme to use" );
        supportsOption( "storegeomids", "Store the citygml id of geometry objects in the corresponding osg::Geometry object as a description string." );

        m_logger = std::make_shared<CityGMLOSGPluginLogger>();
    }

    virtual const char* className( void ) const override { return "CityGML Reader"; }

    virtual ReadResult readNode( const std::string&, const osgDB::ReaderWriter::Options* ) const override;
    virtual ReadResult readNode( std::istream&, const osgDB::ReaderWriter::Options* ) const override;

private:

    std::shared_ptr<citygml::CityGMLLogger> m_logger;
    static unsigned int getHighestLodForObject(const citygml::CityObject& object);

    ReadResult readCity(std::shared_ptr<const citygml::CityModel>, CityGMLSettings& ) const;
    bool createCityObject(const citygml::CityObject&, CityGMLSettings&, osg::Group*, const osg::Vec3d& offset = osg::Vec3d(0.0, 0.0, 0.0), unsigned int minimumLODToConsider = 0) const;
};
