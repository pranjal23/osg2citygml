#include "ReaderWriterCityGML.h"

// Register with Registry to instantiate the above reader/writer.
REGISTER_OSGPLUGIN( citygml, ReaderWriterCityGML )

// Read CityGML file using libcitygml and generate the OSG scenegraph
osgDB::ReaderWriter::ReadResult ReaderWriterCityGML::readNode( const std::string& file, const osgDB::ReaderWriter::Options* options ) const
{
    std::string ext = osgDB::getLowerCaseFileExtension( file );

    if ( !acceptsExtension( ext ) ) return ReadResult::FILE_NOT_HANDLED;

    // try to open the file as is
    std::string fileName = osgDB::findDataFile( file, options );

    if ( fileName.empty() )
    {
        // not found, so remove the .citygml extension file
        std::string fname = osgDB::getNameLessExtension( file );
        fileName = osgDB::findDataFile( fname, options );
        if ( fileName.empty() ) return ReadResult::FILE_NOT_FOUND;
    }

    CityGMLSettings settings;
    settings.parseOptions( options );

    osgDB::getDataFilePathList().push_front( osgDB::getFilePath( fileName ) );

    // Redirect both std::cout & std::cerr (used by libcitygml) to osg::notify stream
    std::streambuf* coutsb = std::cout.rdbuf( osg::notify(osg::NOTICE).rdbuf() );
    std::streambuf* cerrsb = std::cerr.rdbuf( osg::notify(osg::NOTICE).rdbuf() );

    osg::notify(osg::NOTICE) << "Parsing CityGML file " << fileName << "..." << std::endl;

    std::shared_ptr<const citygml::CityModel> city = citygml::load( fileName, settings._params, m_logger );

    ReadResult rr;// = readCity( city, settings );

    if ( rr.status() == ReadResult::FILE_LOADED && rr.getNode() ) {
        rr.getNode()->setName( fileName );

        // Let osg calculate the normals
        osgUtil::SmoothingVisitor sv;
        rr.getNode()->accept(sv);
    }

    osgDB::getDataFilePathList().pop_front();

    // Restore cout/cerr streams
    std::cout.rdbuf( coutsb );
    std::cerr.rdbuf( cerrsb );

    return rr;
}

osgDB::ReaderWriter::ReadResult ReaderWriterCityGML::readNode( std::istream& fin, const osgDB::ReaderWriter::Options* options ) const
{
    CityGMLSettings settings;
    settings.parseOptions( options );

    // Redirect both std::cout & std::cerr (used by CityGML parser) to osg::notify stream
    std::streambuf* coutsb = std::cout.rdbuf( osg::notify(osg::NOTICE).rdbuf() );
    std::streambuf* cerrsb = std::cerr.rdbuf( osg::notify(osg::NOTICE).rdbuf() );

    osg::notify(osg::NOTICE) << "Parsing CityGML stream..." << std::endl;

    std::shared_ptr<const citygml::CityModel> city = citygml::load( fin, settings._params );

    ReadResult rr = readCity( city, settings );

    // Restore cout/cerr streams
    std::cout.rdbuf( coutsb );
    std::cerr.rdbuf( cerrsb );

    return rr;
}

osgDB::ReaderWriter::ReadResult ReaderWriterCityGML::readCity(std::shared_ptr<const citygml::CityModel> city, CityGMLSettings& settings ) const
{
    if ( !city ) return nullptr;

    osg::notify(osg::NOTICE) << city->getNumRootCityObjects() << " city objects read." << std::endl;

    osg::notify(osg::NOTICE) << "Creation of the OSG city objects' geometry..." << std::endl;

    osg::MatrixTransform* root = new osg::MatrixTransform();
    root->setName( city->getId() );

    if (settings._theme.empty() && !city->themes().empty()) {
        settings._theme = city->themes().front();
    }

    const citygml::ConstCityObjects& roots = city->getRootCityObjects();


    if(roots.size() == 0) return nullptr;


    osg::Vec3d offset(0, 0, 0);
    if (city->getEnvelope().validBounds()) {
        TVec3d lb = city->getEnvelope().getLowerBound();
        offset = osg::Vec3d(lb.x, lb.y, lb.z);
    }

    for ( unsigned int i = 0; i < roots.size(); ++i ) createCityObject( *roots[i], settings, root, offset );


    osg::notify(osg::NOTICE) << "Done." << std::endl;
    root->setMatrix(
        osg::Matrixd::translate(offset));

    return root;
}

void setTexture(osg::ref_ptr<osg::StateSet> stateset, osg::Geometry* geom, const citygml::Polygon& polygon, CityGMLSettings& settings) {
    const auto citygmlTex = polygon.getTextureFor(settings._theme);

    if ( !citygmlTex )
    {
        return;
    }
    const std::vector<TVec2f>& texCoords = polygon.getTexCoordsForTheme(settings._theme, true);

    if (texCoords.empty()) {
        osg::notify(osg::WARN) << "Texture coordinates not found for poly " << polygon.getId() << std::endl;
    }

    osg::Texture2D* texture = nullptr;

    if ( settings._textureMap.find( citygmlTex->getUrl() ) == settings._textureMap.end() ) {
        std::string fullPath = osgDB::findDataFile(citygmlTex->getUrl());

        if (fullPath.empty()) {
            osg::notify(osg::NOTICE) << "  Texture file " << citygmlTex->getUrl() << " not found..." << std::endl;
            return;
        }

        // Load a new texture
        osg::notify(osg::NOTICE) << "  Loading texture " << fullPath << "..." << std::endl;

        osg::Image* image = osgDB::readImageFile( citygmlTex->getUrl() );

        if (!image) {
            osg::notify(osg::NOTICE) << "  Warning: Failed to read Texture " << fullPath << std::endl;
            return;
        }

        texture = new osg::Texture2D;
        texture->setImage( image );
        texture->setFilter( osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR );
        texture->setFilter( osg::Texture::MAG_FILTER, osg::Texture::NEAREST );
        texture->setWrap( osg::Texture::WRAP_S, osg::Texture::REPEAT );
        texture->setWrap( osg::Texture::WRAP_T, osg::Texture::REPEAT );
        texture->setWrap( osg::Texture::WRAP_R, osg::Texture::REPEAT );

        settings._textureMap[ citygmlTex->getUrl() ] = texture;
    }
    else {
        texture = settings._textureMap[ citygmlTex->getUrl() ];
    }

    if ( !texture )
    {
        return;
    }

    osg::ref_ptr<osg::Vec2Array> tex = new osg::Vec2Array;

    tex->reserve( texCoords.size() );
    for ( unsigned int k = 0; k < texCoords.size(); k++ )
        tex->push_back( osg::Vec2( texCoords[k].x, texCoords[k].y ) );

    geom->setTexCoordArray( 0, tex );

    stateset->setTextureAttributeAndModes( 0, texture, osg::StateAttribute::ON );
}

void setMaterial(osg::ref_ptr<osg::StateSet> stateset, const citygml::Polygon& polygon, CityGMLSettings& settings) {

    const auto citygmlMaterial = polygon.getMaterialFor(settings._theme);

    if (!citygmlMaterial) {
        return;
    }

    TVec4f diffuse( citygmlMaterial->getDiffuse(), 0.f );
    TVec4f emissive( citygmlMaterial->getEmissive(), 0.f );
    TVec4f specular( citygmlMaterial->getSpecular(), 0.f );
    float ambient = citygmlMaterial->getAmbientIntensity();

    osg::Material* material = new osg::Material;
    material->setColorMode( osg::Material::OFF );
    material->setDiffuse( osg::Material::FRONT_AND_BACK, osg::Vec4(diffuse.r, diffuse.g, diffuse.b, diffuse.a ) );
    material->setSpecular( osg::Material::FRONT_AND_BACK, osg::Vec4(specular.r, specular.g, specular.b, specular.a ) );
    material->setEmission( osg::Material::FRONT_AND_BACK, osg::Vec4(emissive.r, emissive.g, emissive.b, emissive.a ) );
    material->setShininess( osg::Material::FRONT_AND_BACK, 128.f * citygmlMaterial->getShininess() );
    material->setAmbient( osg::Material::FRONT_AND_BACK, osg::Vec4( ambient, ambient, ambient, 1.0 ) );
    material->setTransparency( osg::Material::FRONT_AND_BACK, citygmlMaterial->getTransparency() );
    stateset->setAttributeAndModes( material, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON );
    stateset->setMode( GL_LIGHTING, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON );
}

void createOsgGeometryFromCityGMLGeometry(const citygml::Geometry& geometry, CityGMLSettings& settings, osg::Geode* geometryContainer, const osg::Vec3d& offset ) {
    for ( unsigned int j = 0; j < geometry.getPolygonsCount(); j++ )
    {
        const citygml::Polygon& p = *geometry.getPolygon(j);

        if ( p.getIndices().size() == 0 ) continue;

        // Geometry management

        osg::Geometry* geom = new osg::Geometry;
        geom->setName( p.getId() );
        geom->setUserValue("cot_type", geometry.getTypeAsString());

        // Vertices
        osg::Vec3Array* vertices = new osg::Vec3Array;
        const std::vector<TVec3d>& vert = p.getVertices();
        vertices->reserve( vert.size() );
        for ( unsigned int k = 0; k < vert.size(); k++ )
        {
            TVec3d v = vert[k];
            osg::Vec3d pt = osg::Vec3d( v.x, v.y, v.z ) - offset;
            vertices->push_back( pt );
        }

        geom->setVertexArray( vertices );

        // Indices
        osg::DrawElementsUInt* indices = new osg::DrawElementsUInt( osg::PrimitiveSet::TRIANGLES, p.getIndices().begin(), p.getIndices().end());
        geom->addPrimitiveSet( indices );

        // Appearance

        osg::ref_ptr<osg::StateSet> stateset = geom->getOrCreateStateSet();

        setMaterial(stateset, p, settings);
        setTexture(stateset, geom, p, settings);

#if OSG_VERSION_GREATER_OR_EQUAL(3,3,2)
            if (settings._storeGeomIDs) {
                geom->addDescription(p.getId());
            }
#endif


        geometryContainer->addDrawable( geom );
    }

    // Parse child geoemtries
    for (unsigned int i = 0; i < geometry.getGeometriesCount(); i++) {
        createOsgGeometryFromCityGMLGeometry(geometry.getGeometry(i), settings, geometryContainer, offset);
    }
}

bool ReaderWriterCityGML::createCityObject(const citygml::CityObject& object, CityGMLSettings& settings, osg::Group* parent, const osg::Vec3d& offset , unsigned int minimumLODToConsider) const
{
    // Skip objects without geometry
    if ( !parent ) return false;

    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    geode->setName( object.getId() );

    osg::Group* grp = new osg::Group;
    grp->setName( object.getId() );
    grp->setUserValue("cot_type", object.getTypeAsString());
    grp->addChild( geode );
    parent->addChild( grp );

    osg::ref_ptr<osg::Vec4Array> roof_color = new osg::Vec4Array;
    roof_color->push_back( osg::Vec4( 0.9f, 0.1f, 0.1f, 1.0f ) );

    unsigned int highestLOD = ReaderWriterCityGML::getHighestLodForObject(object);

    for ( unsigned int i = 0; i < object.getGeometriesCount(); i++ )
    {
        const citygml::Geometry& geometry = object.getGeometry( i );

        const unsigned int currentLOD = geometry.getLOD();

        if (settings._useMaxLODOnly && (currentLOD < highestLOD || currentLOD < minimumLODToConsider )){
            continue;
        }

        createOsgGeometryFromCityGMLGeometry(geometry, settings, geode, offset);
    }

    if ( settings._printNames )
    {
        // Print the city object name on top of it
        geode->getBoundingBox().center();
        osg::ref_ptr<osgText::Text> text = new osgText::Text;
        text->setFont( "arial.ttf" );
        text->setCharacterSize( 2 );
        text->setBackdropType( osgText::Text::OUTLINE );
        text->setFontResolution( 64, 64 );
        text->setText( object.getId(), osgText::String::ENCODING_UTF8 );
        text->setCharacterSizeMode( osgText::TextBase::OBJECT_COORDS_WITH_MAXIMUM_SCREEN_SIZE_CAPPED_BY_FONT_HEIGHT );
        text->setAxisAlignment( osgText::TextBase::SCREEN );
        text->setAlignment( osgText::TextBase::CENTER_BOTTOM );
        text->setPosition( geode->getBoundingBox().center() + osg::Vec3( 0, 0, geode->getBoundingBox().radius() ) );
        text->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OVERRIDE|osg::StateAttribute::OFF );
        geode->addDrawable( text.get() );
    }

    // Manage transparency for windows
    if ( object.getType() == citygml::CityObject::CityObjectsType::COT_Window )
    {
        osg::StateSet *geodeSS( geode->getOrCreateStateSet() );

        osg::ref_ptr<osg::BlendFunc> blendFunc = new osg::BlendFunc(osg::BlendFunc::ONE_MINUS_CONSTANT_ALPHA,osg::BlendFunc::CONSTANT_ALPHA);
        geodeSS->setAttributeAndModes( blendFunc.get(), osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON );

        osg::ref_ptr<osg::BlendColor> blendColor = new osg::BlendColor(osg::Vec4( 1., 1., 1., 0.4 ));
        geodeSS->setAttributeAndModes( blendColor.get(), osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON );

        osg::ref_ptr<osg::Depth> depth = new osg::Depth;
        depth->setWriteMask( false );
        geodeSS->setAttributeAndModes( depth.get(), osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON );

        geodeSS->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
    }

    for ( unsigned int i = 0; i < object.getChildCityObjectsCount(); ++i )
        createCityObject( object.getChildCityObject(i), settings, grp, offset, highestLOD);

    return true;
}

unsigned int ReaderWriterCityGML::getHighestLodForObject( const citygml::CityObject& object){
    unsigned int highestLOD = 0;
    // first find out highest LOD for this object
    for (unsigned int i = 0; i < object.getGeometriesCount(); i++) {
        const citygml::Geometry &geometry = object.getGeometry(i);

        if (geometry.getLOD() > highestLOD){
            highestLOD = geometry.getLOD();
        }
    }

    //check for the highest LODs of Children
    for (unsigned int i = 0; i < object.getChildCityObjectsCount(); ++i){
        unsigned int tempHighestLOD = ReaderWriterCityGML::getHighestLodForObject(object.getChildCityObject(i));
        if (tempHighestLOD > highestLOD){
            tempHighestLOD = highestLOD;
        }
    }

    return highestLOD;
}
