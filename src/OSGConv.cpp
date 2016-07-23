#include "OSGConv.h"

static void usage( const char *prog, const char *msg )
{
    if (msg)
    {
        osg::notify(osg::NOTICE)<< std::endl;
        osg::notify(osg::NOTICE) << msg << std::endl;
    }

    // basic usage
    osg::notify(osg::NOTICE)<< std::endl;
    osg::notify(osg::NOTICE)<<"usage:"<< std::endl;
    osg::notify(osg::NOTICE)<<"    " << prog << " [options] infile1 [infile2 ...] outfile"<< std::endl;
    osg::notify(osg::NOTICE)<< std::endl;

    // print env options - especially since optimizer is always _on_
    osg::notify(osg::NOTICE)<<"environment:" << std::endl;
    osg::ApplicationUsage::UsageMap um = osg::ApplicationUsage::instance()->getEnvironmentalVariables();
    std::string envstring;
    osg::ApplicationUsage::instance()->getFormattedString( envstring, um );
    osg::notify(osg::NOTICE)<<envstring << std::endl;

    // print tool options
    osg::notify(osg::NOTICE)<<"options:"<< std::endl;
    osg::notify(osg::NOTICE)<<"    -O option          - ReaderWriter option"<< std::endl;
    osg::notify(osg::NOTICE)<< std::endl;
    osg::notify(osg::NOTICE)<<"    --compressed       - Enable the usage of compressed textures,"<< std::endl;
    osg::notify(osg::NOTICE)<<"                         defaults to OpenGL ARB compressed textures."<< std::endl;
    osg::notify(osg::NOTICE)<<"    --compressed-arb   - Enable the usage of OpenGL ARB compressed textures"<< std::endl;
    osg::notify(osg::NOTICE)<<"    --compressed-dxt1  - Enable the usage of S3TC DXT1 compressed textures"<< std::endl;
    osg::notify(osg::NOTICE)<<"    --compressed-dxt3  - Enable the usage of S3TC DXT3 compressed textures"<< std::endl;
    osg::notify(osg::NOTICE)<<"    --compressed-dxt5  - Enable the usage of S3TC DXT5 compressed textures"<< std::endl;
    osg::notify(osg::NOTICE)<< std::endl;
    osg::notify(osg::NOTICE)<<"    --fix-transparency - fix statesets which are currently"<< std::endl;
    osg::notify(osg::NOTICE)<<"                         declared as transparent, but should be opaque."<< std::endl;
    osg::notify(osg::NOTICE)<<"                         Defaults to using the fixTranspancyMode"<< std::endl;
    osg::notify(osg::NOTICE)<<"                         MAKE_OPAQUE_TEXTURE_STATESET_OPAQUE."<< std::endl;
    osg::notify(osg::NOTICE)<<"    --fix-transparency-mode <mode_string>  - fix statesets which are currently"<< std::endl;
    osg::notify(osg::NOTICE)<<"                         declared as transparent but should be opaque."<< std::endl;
    osg::notify(osg::NOTICE)<<"                         The mode_string determines which algorithm is used"<< std::endl;
    osg::notify(osg::NOTICE)<<"                         to fix the transparency, options are:"<< std::endl;
    osg::notify(osg::NOTICE)<<"                                 MAKE_OPAQUE_TEXTURE_STATESET_OPAQUE,"<<std::endl;
    osg::notify(osg::NOTICE)<<"                                 MAKE_ALL_STATESET_OPAQUE."<<std::endl;

    osg::notify(osg::NOTICE)<< std::endl;
    osg::notify(osg::NOTICE)<<"    -l libraryName     - load plugin of name libraryName"<< std::endl;
    osg::notify(osg::NOTICE)<<"                         i.e. -l osgdb_pfb"<< std::endl;
    osg::notify(osg::NOTICE)<<"                         Useful for loading reader/writers which can load"<< std::endl;
    osg::notify(osg::NOTICE)<<"                         other file formats in addition to its extension."<< std::endl;
    osg::notify(osg::NOTICE)<<"    -e extensionName   - load reader/wrter plugin for file extension"<< std::endl;
    osg::notify(osg::NOTICE)<<"                         i.e. -e pfb"<< std::endl;
    osg::notify(osg::NOTICE)<<"                         Useful short hand for specifying full library name as"<< std::endl;
    osg::notify(osg::NOTICE)<<"                         done with -l above, as it automatically expands to the"<< std::endl;
    osg::notify(osg::NOTICE)<<"                         full library name appropriate for each platform."<< std::endl;
    osg::notify(osg::NOTICE)<<"    -o orientation     - Convert geometry from input files to output files."<< std::endl;
    osg::notify(osg::NOTICE)<<
                               "                         Format of orientation argument must be the following:\n"
                               "\n"
                               "                             X1,Y1,Z1-X2,Y2,Z2\n"
                               "                         or\n"
                               "                             degrees-A0,A1,A2\n"
                               "\n"
                               "                         where X1,Y1,Z1 represent the UP vector in the input\n"
                               "                         files and X2,Y2,Z2 represent the UP vector of the\n"
                               "                         output file, or degrees is the rotation angle in\n"
                               "                         degrees around axis (A0,A1,A2).  For example, to\n"
                               "                         convert a model built in a Y-Up coordinate system to a\n"
                               "                         model with a Z-up coordinate system, the argument may\n"
                               "                         look like\n"
                               "\n"
                               "                             0,1,0-0,0,1"
                               "\n"
                               "                          or\n"
                               "                             -90-1,0,0\n"
                               "\n" << std::endl;
    osg::notify(osg::NOTICE)<<"    -t translation     - Convert spatial position of output files.  Format of\n"
                              "                         translation argument must be the following :\n"
                              "\n"
                              "                             X,Y,Z\n"
                              "\n"
                              "                         where X, Y, and Z represent the coordinates of the\n"
                              "                         absolute position in world space\n"
                           << std::endl;
    osg::notify(osg::NOTICE)<<"    --use-world-frame  - Perform transformations in the world frame, rather\n"
                              "                         than relative to the center of the bounding sphere.\n"
                           << std::endl;
    osg::notify(osg::NOTICE)<<"    --simplify n       - Run simplifier prior to output. Argument must be a" << std::endl
                           <<"                         normalized value for the resultant percentage" << std::endl
                          <<"                         reduction." << std::endl
                         <<"                         Example: --simplify .5" << std::endl
                        <<"                                 will produce a 50% reduced model." << std::endl
                       << std::endl;
    osg::notify(osg::NOTICE)<<"    -s scale           - Scale size of model.  Scale argument must be the \n"
                              "                         following :\n"
                              "\n"
                              "                             SX,SY,SZ\n"
                              "\n"
                              "                         where SX, SY, and SZ represent the scale factors\n"
                              "                         Caution: Scaling is done in destination orientation\n"
                           << std::endl;
    osg::notify(osg::NOTICE)<<"    --smooth           - Smooth the surface by regenerating surface normals on\n"
                              "                         all geometry nodes"<< std::endl;
    osg::notify(osg::NOTICE)<<"    --addMissingColors - Add a white color value to all geometry nodes\n"
                              "                         that don't have their own color values\n"
                              "                         (--addMissingColours also accepted)."<< std::endl;
    osg::notify(osg::NOTICE)<<"    --overallNormal    - Replace normals with a single overall normal."<< std::endl;
    osg::notify(osg::NOTICE)<<"    --enable-object-cache - Enable caching of objects, images, etc."<< std::endl;

    osg::notify( osg::NOTICE ) << std::endl;
    osg::notify( osg::NOTICE ) <<
                                  "    --formats          - List all supported formats and their supported options." << std::endl;
    osg::notify( osg::NOTICE ) <<
                                  "    --format <format>  - Display information about the specified <format>,\n"
                                  "                         where <format> is the file extension, such as \"flt\"." << std::endl;
    osg::notify( osg::NOTICE ) <<
                                  "    --plugins          - List all supported plugin files." << std::endl;
    osg::notify( osg::NOTICE ) <<
                                  "    --plugin <plugin>  - Display information about the specified <plugin>,\n"
                                  "                         where <plugin> is the plugin's full path and file name." << std::endl;
}


osg::ref_ptr<osg::Node> readModel( int argc, char **argv )
{
    // use an ArgumentParser object to manage the program arguments.
    osg::ArgumentParser arguments(&argc,argv);

    // set up the usage document, in case we need to print out how to use this program.
    arguments.getApplicationUsage()->setApplicationName(arguments.getApplicationName());
    arguments.getApplicationUsage()->setDescription(arguments.getApplicationName()+" is a utility for converting between various input and output databases formats.");
    arguments.getApplicationUsage()->setCommandLineUsage(arguments.getApplicationName()+" [options] filename ...");
    arguments.getApplicationUsage()->addCommandLineOption("-h or --help","Display command line parameters");
    arguments.getApplicationUsage()->addCommandLineOption("--help-env","Display environmental variables available");


    // if user request help write it out to cout.
    if (arguments.read("-h") || arguments.read("--help"))
    {
        osg::setNotifyLevel(osg::NOTICE);
        usage( arguments.getApplicationName().c_str(), 0 );
        //arguments.getApplicationUsage()->write(std::cout);
        return NULL;
    }

    if (arguments.read("--help-env"))
    {
        arguments.getApplicationUsage()->write(std::cout, osg::ApplicationUsage::ENVIRONMENTAL_VARIABLE);
        return NULL;
    }

    if (arguments.read("--plugins"))
    {
        osgDB::FileNameList plugins = osgDB::listAllAvailablePlugins();
        for(osgDB::FileNameList::iterator itr = plugins.begin();
            itr != plugins.end();
            ++itr)
        {
            std::cout<<"Plugin "<<*itr<<std::endl;
        }
        return NULL;
    }

    std::string plugin;
    if (arguments.read("--plugin", plugin))
    {
        osgDB::outputPluginDetails(std::cout, plugin);
        return NULL;
    }

    std::string ext;
    if (arguments.read("--format", ext))
    {
        plugin = osgDB::Registry::instance()->createLibraryNameForExtension(ext);
        osgDB::outputPluginDetails(std::cout, plugin);
        return NULL;
    }

    if (arguments.read("--formats"))
    {
        osgDB::FileNameList plugins = osgDB::listAllAvailablePlugins();
        for(osgDB::FileNameList::iterator itr = plugins.begin();
            itr != plugins.end();
            ++itr)
        {
            osgDB::outputPluginDetails(std::cout,*itr);
        }
        return NULL;
    }

    if (arguments.argc()<=1)
    {
        arguments.getApplicationUsage()->write(std::cout,osg::ApplicationUsage::COMMAND_LINE_OPTION);
        return NULL;
    }

    FileNameList fileNames;
    OrientationConverter oc;
    bool do_convert = false;

    if (arguments.read("--use-world-frame"))
    {
        oc.useWorldFrame(true);
    }

    std::string str;
    while (arguments.read("-O",str))
    {
        osgDB::ReaderWriter::Options* options = new osgDB::ReaderWriter::Options;
        options->setOptionString(str);
        osgDB::Registry::instance()->setOptions(options);
    }

    while (arguments.read("-e",ext))
    {
        std::string libName = osgDB::Registry::instance()->createLibraryNameForExtension(ext);
        osgDB::Registry::instance()->loadLibrary(libName);
    }

    std::string libName;
    while (arguments.read("-l",libName))
    {
        osgDB::Registry::instance()->loadLibrary(libName);
    }

    while (arguments.read("-o",str))
    {
        osg::Vec3 from, to;
        if( sscanf( str.c_str(), "%f,%f,%f-%f,%f,%f",
                    &from[0], &from[1], &from[2],
                    &to[0], &to[1], &to[2]  )
                != 6 )
        {
            float degrees;
            osg::Vec3 axis;
            // Try deg-axis format
            if( sscanf( str.c_str(), "%f-%f,%f,%f",
                        &degrees, &axis[0], &axis[1], &axis[2]  ) != 4 )
            {
                usage( argv[0], "Orientation argument format incorrect." );
                return NULL;
            }
            else
            {
                oc.setRotation( degrees, axis );
                do_convert = true;
            }
        }
        else
        {
            oc.setRotation( from, to );
            do_convert = true;
        }
    }

    while (arguments.read("-s",str))
    {
        osg::Vec3 scale(0,0,0);
        if( sscanf( str.c_str(), "%f,%f,%f",
                    &scale[0], &scale[1], &scale[2] ) != 3 )
        {
            usage( argv[0], "Scale argument format incorrect." );
            return NULL;
        }
        oc.setScale( scale );
        do_convert = true;
    }

    float simplifyPercent = 1.0;
    bool do_simplify = false;
    while ( arguments.read( "--simplify",str ) )
    {
        float nsimp = 1.0;
        if( sscanf( str.c_str(), "%f",
                    &nsimp ) != 1 )
        {
            usage( argv[0], "Scale argument format incorrect." );
            return NULL;
        }
        std::cout << str << " " << nsimp << std::endl;
        simplifyPercent = nsimp;
        osg::notify( osg::INFO ) << "Simplifying with percentage: " << simplifyPercent << std::endl;
        do_simplify = true;
    }

    while (arguments.read("-t",str))
    {
        osg::Vec3 trans(0,0,0);
        if( sscanf( str.c_str(), "%f,%f,%f",
                    &trans[0], &trans[1], &trans[2] ) != 3 )
        {
            usage( argv[0], "Translation argument format incorrect." );
            return NULL;
        }
        oc.setTranslation( trans );
        do_convert = true;
    }


    FixTransparencyVisitor::FixTransparencyMode fixTransparencyMode = FixTransparencyVisitor::NO_TRANSPARANCY_FIXING;
    std::string fixString;
    while(arguments.read("--fix-transparency")) fixTransparencyMode = FixTransparencyVisitor::MAKE_OPAQUE_TEXTURE_STATESET_OPAQUE;
    while(arguments.read("--fix-transparency-mode",fixString))
    {
        if (fixString=="MAKE_OPAQUE_TEXTURE_STATESET_OPAQUE") fixTransparencyMode = FixTransparencyVisitor::MAKE_OPAQUE_TEXTURE_STATESET_OPAQUE;
        if (fixString=="MAKE_ALL_STATESET_OPAQUE") fixTransparencyMode = FixTransparencyVisitor::MAKE_ALL_STATESET_OPAQUE;
    };

    bool pruneStateSet = false;
    while(arguments.read("--prune-StateSet")) pruneStateSet = true;

    osg::Texture::InternalFormatMode internalFormatMode = osg::Texture::USE_IMAGE_DATA_FORMAT;
    while(arguments.read("--compressed") || arguments.read("--compressed-arb")) { internalFormatMode = osg::Texture::USE_ARB_COMPRESSION; }

    while(arguments.read("--compressed-dxt1")) { internalFormatMode = osg::Texture::USE_S3TC_DXT1_COMPRESSION; }
    while(arguments.read("--compressed-dxt3")) { internalFormatMode = osg::Texture::USE_S3TC_DXT3_COMPRESSION; }
    while(arguments.read("--compressed-dxt5")) { internalFormatMode = osg::Texture::USE_S3TC_DXT5_COMPRESSION; }

    bool smooth = false;
    while(arguments.read("--smooth")) { smooth = true; }

    bool addMissingColours = false;
    while(arguments.read("--addMissingColours") || arguments.read("--addMissingColors")) { addMissingColours = true; }

    bool do_overallNormal = false;
    while(arguments.read("--overallNormal") || arguments.read("--overallNormal")) { do_overallNormal = true; }

    bool enableObjectCache = false;
    while(arguments.read("--enable-object-cache")) { enableObjectCache = true; }

    // any option left unread are converted into errors to write out later.
    arguments.reportRemainingOptionsAsUnrecognized();

    // report any errors if they have occurred when parsing the program arguments.
    if (arguments.errors())
    {
        arguments.writeErrorMessages(std::cout);
        return NULL;
    }

    for(int pos=1;pos<arguments.argc();++pos)
    {
        if (!arguments.isOption(pos))
        {
            fileNames.push_back(arguments[pos]);
        }
    }

    if (enableObjectCache)
    {
        if (osgDB::Registry::instance()->getOptions()==0) osgDB::Registry::instance()->setOptions(new osgDB::Options());
        osgDB::Registry::instance()->getOptions()->setObjectCacheHint(osgDB::Options::CACHE_ALL);
    }

    std::string fileNameOut("converted.osg");
    if (fileNames.size()>1)
    {
        fileNameOut = fileNames.back();
        fileNames.pop_back();
    }

    osg::Timer_t startTick = osg::Timer::instance()->tick();

    osg::ref_ptr<osg::Node> root = osgDB::readNodeFiles(fileNames);

    if (root.valid())
    {
        osg::Timer_t endTick = osg::Timer::instance()->tick();
        osg::notify(osg::INFO)<<"Time to load files "<<osg::Timer::instance()->delta_m(startTick, endTick)<<" ms"<<std::endl;
    }


    if (pruneStateSet)
    {
        PruneStateSetVisitor pssv;
        root->accept(pssv);
    }

    if (fixTransparencyMode != FixTransparencyVisitor::NO_TRANSPARANCY_FIXING)
    {
        FixTransparencyVisitor atv(fixTransparencyMode);
        root->accept(atv);
    }

    if ( root.valid() )
    {

        if (smooth)
        {
            osgUtil::SmoothingVisitor sv;
            root->accept(sv);
        }

        if (addMissingColours)
        {
            AddMissingColoursToGeometryVisitor av;
            root->accept(av);
        }

        // optimize the scene graph, remove redundant nodes and state etc.
        osgUtil::Optimizer optimizer;
        optimizer.optimize(root.get());

        if( do_convert )
            root = oc.convert( root.get() );

        if (internalFormatMode != osg::Texture::USE_IMAGE_DATA_FORMAT)
        {
            std::string ext = osgDB::getFileExtension(fileNameOut);
            CompressTexturesVisitor ctv(internalFormatMode);
            root->accept(ctv);
            ctv.compress();

            osgDB::ReaderWriter::Options *options = osgDB::Registry::instance()->getOptions();
            if (ext!="ive" || (options && options->getOptionString().find("noTexturesInIVEFile")!=std::string::npos))
            {
                ctv.write(osgDB::getFilePath(fileNameOut));
            }
        }

        // scrub normals
        if ( do_overallNormal )
        {
            DefaultNormalsGeometryVisitor dngv;
            root->accept( dngv );
        }

        // apply any user-specified simplification
        if ( do_simplify )
        {
            osgUtil::Simplifier simple;
            simple.setSmoothing( smooth );
            osg::notify( osg::ALWAYS ) << " smoothing: " << smooth << std::endl;
            simple.setSampleRatio( simplifyPercent );
            root->accept( simple );
        }
        return root;

        /*
        osgDB::ReaderWriter::WriteResult result = osgDB::Registry::instance()->writeNode(*root,fileNameOut,osgDB::Registry::instance()->getOptions());
        if (result.success())
        {
            osg::notify(osg::NOTICE)<<"Data written to '"<<fileNameOut<<"'."<< std::endl;
        }
        else if  (result.message().empty())
        {
            osg::notify(osg::NOTICE)<<"Warning: file write to '"<<fileNameOut<<"' not supported."<< std::endl;
        }
        else
        {
            osg::notify(osg::NOTICE)<<result.message()<< std::endl;
        }
        */
    }
    else
    {
        osg::notify(osg::NOTICE)<<"Error no data loaded."<< std::endl;
        return NULL;
    }

    return NULL;
}
