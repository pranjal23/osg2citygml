#ifndef OSGConv_h__
#define OSGConv_h__

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
#include "OrientationConverter.h"


typedef std::vector<std::string> FileNameList;

class MyGraphicsContext {
public:
    MyGraphicsContext()
    {
        osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
        traits->x = 0;
        traits->y = 0;
        traits->width = 1;
        traits->height = 1;
        traits->windowDecoration = false;
        traits->doubleBuffer = false;
        traits->sharedContext = 0;
        traits->pbuffer = true;

        _gc = osg::GraphicsContext::createGraphicsContext(traits.get());

        if (!_gc)
        {
            osg::notify(osg::NOTICE)<<"Failed to create pbuffer, failing back to normal graphics window."<<std::endl;

            traits->pbuffer = false;
            _gc = osg::GraphicsContext::createGraphicsContext(traits.get());
        }

        if (_gc.valid())
        {
            _gc->realize();
            _gc->makeCurrent();
            if (dynamic_cast<osgViewer::GraphicsWindow*>(_gc.get()))
            {
                std::cout<<"Realized graphics window for OpenGL operations."<<std::endl;
            }
            else
            {
                std::cout<<"Realized pbuffer for OpenGL operations."<<std::endl;
            }
        }
    }

    bool valid() const { return _gc.valid() && _gc->isRealized(); }

private:
    osg::ref_ptr<osg::GraphicsContext> _gc;
};

class DefaultNormalsGeometryVisitor
        : public osg::NodeVisitor
{
public:

    DefaultNormalsGeometryVisitor()
        : osg::NodeVisitor( osg::NodeVisitor::TRAVERSE_ALL_CHILDREN ) {
    }

    virtual void apply( osg::Geode & geode )
    {
        for( unsigned int ii = 0; ii < geode.getNumDrawables(); ++ii )
        {
            osg::ref_ptr< osg::Geometry > geometry = dynamic_cast< osg::Geometry * >( geode.getDrawable( ii ) );
            if( geometry.valid() )
            {
                osg::ref_ptr< osg::Vec3Array > newnormals = new osg::Vec3Array;
                newnormals->push_back( osg::Z_AXIS );
                geometry->setNormalArray( newnormals.get(), osg::Array::BIND_OVERALL );
            }
        }
    }

    virtual void apply( osg::Node & node )
    {
        traverse( node );
    }

};

class CompressTexturesVisitor : public osg::NodeVisitor
{
public:

    CompressTexturesVisitor(osg::Texture::InternalFormatMode internalFormatMode):
        osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN),
        _internalFormatMode(internalFormatMode) {}

    virtual void apply(osg::Node& node)
    {
        if (node.getStateSet()) apply(*node.getStateSet());
        traverse(node);
    }

    virtual void apply(osg::Geode& node)
    {
        if (node.getStateSet()) apply(*node.getStateSet());

        for(unsigned int i=0;i<node.getNumDrawables();++i)
        {
            osg::Drawable* drawable = node.getDrawable(i);
            if (drawable && drawable->getStateSet()) apply(*drawable->getStateSet());
        }

        traverse(node);
    }

    virtual void apply(osg::StateSet& stateset)
    {
        // search for the existence of any texture object attributes
        for(unsigned int i=0;i<stateset.getTextureAttributeList().size();++i)
        {
            osg::Texture* texture = dynamic_cast<osg::Texture*>(stateset.getTextureAttribute(i,osg::StateAttribute::TEXTURE));
            if (texture)
            {
                _textureSet.insert(texture);
            }
        }
    }

    void compress()
    {
        MyGraphicsContext context;
        if (!context.valid())
        {
            osg::notify(osg::NOTICE)<<"Error: Unable to create graphis context, problem with running osgViewer-"<<osgViewerGetVersion()<<", cannot run compression."<<std::endl;
            return;
        }

        osg::ref_ptr<osg::State> state = new osg::State;
        state->initializeExtensionProcs();

        for(TextureSet::iterator itr=_textureSet.begin();
            itr!=_textureSet.end();
            ++itr)
        {
            osg::Texture* texture = const_cast<osg::Texture*>(itr->get());

            osg::Texture2D* texture2D = dynamic_cast<osg::Texture2D*>(texture);
            osg::Texture3D* texture3D = dynamic_cast<osg::Texture3D*>(texture);

            osg::ref_ptr<osg::Image> image = texture2D ? texture2D->getImage() : (texture3D ? texture3D->getImage() : 0);
            if (image.valid() &&
                    (image->getPixelFormat()==GL_RGB || image->getPixelFormat()==GL_RGBA) &&
                    (image->s()>=32 && image->t()>=32))
            {
                texture->setInternalFormatMode(_internalFormatMode);

                // need to disable the unref after apply, otherwise the image could go out of scope.
                bool unrefImageDataAfterApply = texture->getUnRefImageDataAfterApply();
                texture->setUnRefImageDataAfterApply(false);

                // get OpenGL driver to create texture from image.
                texture->apply(*state);

                // restore the original setting
                texture->setUnRefImageDataAfterApply(unrefImageDataAfterApply);

                image->readImageFromCurrentTexture(0,true);

                texture->setInternalFormatMode(osg::Texture::USE_IMAGE_DATA_FORMAT);
            }
        }
    }

    void write(const std::string &dir)
    {
        for(TextureSet::iterator itr=_textureSet.begin();
            itr!=_textureSet.end();
            ++itr)
        {
            osg::Texture* texture = const_cast<osg::Texture*>(itr->get());

            osg::Texture2D* texture2D = dynamic_cast<osg::Texture2D*>(texture);
            osg::Texture3D* texture3D = dynamic_cast<osg::Texture3D*>(texture);

            osg::ref_ptr<osg::Image> image = texture2D ? texture2D->getImage() : (texture3D ? texture3D->getImage() : 0);
            if (image.valid())
            {
                std::string name = osgDB::getStrippedName(image->getFileName());
                name += ".dds";
                image->setFileName(name);
                std::string path = dir.empty() ? name : osgDB::concatPaths(dir, name);
                osgDB::writeImageFile(*image, path);
                osg::notify(osg::NOTICE) << "Image written to '" << path << "'." << std::endl;
            }
        }
    }

    typedef std::set< osg::ref_ptr<osg::Texture> > TextureSet;
    TextureSet                          _textureSet;
    osg::Texture::InternalFormatMode    _internalFormatMode;

};


class FixTransparencyVisitor : public osg::NodeVisitor
{
public:

    enum FixTransparencyMode
    {
        NO_TRANSPARANCY_FIXING,
        MAKE_OPAQUE_TEXTURE_STATESET_OPAQUE,
        MAKE_ALL_STATESET_OPAQUE
    };

    FixTransparencyVisitor(FixTransparencyMode mode=MAKE_OPAQUE_TEXTURE_STATESET_OPAQUE):
        osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN),
        _numTransparent(0),
        _numOpaque(0),
        _numTransparentMadeOpaque(0),
        _mode(mode)
    {
        std::cout<<"Running FixTransparencyVisitor..."<<std::endl;
    }

    ~FixTransparencyVisitor()
    {
        std::cout<<"  Number of Transparent StateSet "<<_numTransparent<<std::endl;
        std::cout<<"  Number of Opaque StateSet "<<_numOpaque<<std::endl;
        std::cout<<"  Number of Transparent State made Opaque "<<_numTransparentMadeOpaque<<std::endl;
    }

    virtual void apply(osg::Node& node)
    {
        if (node.getStateSet()) isTransparent(*node.getStateSet());
        traverse(node);
    }

    virtual void apply(osg::Geode& node)
    {
        if (node.getStateSet()) isTransparent(*node.getStateSet());

        for(unsigned int i=0;i<node.getNumDrawables();++i)
        {
            osg::Drawable* drawable = node.getDrawable(i);
            if (drawable && drawable->getStateSet()) isTransparent(*drawable->getStateSet());
        }

        traverse(node);
    }

    virtual bool isTransparent(osg::StateSet& stateset)
    {
        bool hasTranslucentTexture = false;
        bool hasBlendFunc = dynamic_cast<osg::BlendFunc*>(stateset.getAttribute(osg::StateAttribute::BLENDFUNC))!=0;
        bool hasTransparentRenderingHint = stateset.getRenderingHint()==osg::StateSet::TRANSPARENT_BIN;
        bool hasDepthSortBin = (stateset.getRenderBinMode()==osg::StateSet::USE_RENDERBIN_DETAILS)?(stateset.getBinName()=="DepthSortedBin"):false;
        bool hasTexture = false;


        // search for the existence of any texture object attributes
        for(unsigned int i=0;i<stateset.getTextureAttributeList().size();++i)
        {
            osg::Texture* texture = dynamic_cast<osg::Texture*>(stateset.getTextureAttribute(i,osg::StateAttribute::TEXTURE));
            if (texture)
            {
                hasTexture = true;
                for (unsigned int im=0;im<texture->getNumImages();++im)
                {
                    osg::Image* image = texture->getImage(im);
                    if (image && image->isImageTranslucent()) hasTranslucentTexture = true;
                }
            }
        }

        if (hasTranslucentTexture || hasBlendFunc || hasTransparentRenderingHint || hasDepthSortBin)
        {
            ++_numTransparent;

            bool makeNonTransparent = false;

            switch(_mode)
            {
            case(MAKE_OPAQUE_TEXTURE_STATESET_OPAQUE):
                if (hasTexture && !hasTranslucentTexture)
                {
                    makeNonTransparent = true;
                }
                break;
            case(MAKE_ALL_STATESET_OPAQUE):
                makeNonTransparent = true;
                break;
            default:
                makeNonTransparent = false;
                break;
            }

            if (makeNonTransparent)
            {
                stateset.removeAttribute(osg::StateAttribute::BLENDFUNC);
                stateset.removeMode(GL_BLEND);
                stateset.setRenderingHint(osg::StateSet::DEFAULT_BIN);
                ++_numTransparentMadeOpaque;
            }


            return true;
        }
        else
        {
            ++_numOpaque;
            return false;
        }
    }

    unsigned int _numTransparent;
    unsigned int _numOpaque;
    unsigned int _numTransparentMadeOpaque;
    FixTransparencyMode _mode;
};

class PruneStateSetVisitor : public osg::NodeVisitor
{
public:

    PruneStateSetVisitor():
        osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN),
        _numStateSetRemoved(0)
    {
        std::cout<<"Running PruneStateSet..."<<std::endl;
    }

    ~PruneStateSetVisitor()
    {
        std::cout<<"  Number of StateState removed "<<_numStateSetRemoved<<std::endl;
    }

    virtual void apply(osg::Node& node)
    {
        if (node.getStateSet())
        {
            node.setStateSet(0);
            ++_numStateSetRemoved;
        }
        traverse(node);
    }

    virtual void apply(osg::Geode& node)
    {
        if (node.getStateSet())
        {
            node.setStateSet(0);
            ++_numStateSetRemoved;
        }

        traverse(node);
    }

    unsigned int _numStateSetRemoved;
};

/** Add missing colours to osg::Geometry.*/
class AddMissingColoursToGeometryVisitor : public osg::NodeVisitor
{
public:

    AddMissingColoursToGeometryVisitor():osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) {}

    virtual void apply(osg::Geode& geode)
    {
        for(unsigned int i=0;i<geode.getNumDrawables();++i)
        {
            osg::Geometry* geometry = dynamic_cast<osg::Geometry*>(geode.getDrawable(i));
            if (geometry)
            {
                if (geometry->getColorArray()==0 || geometry->getColorArray()->getNumElements()==0)
                {
                    osg::Vec4Array* colours = new osg::Vec4Array(1);
                    (*colours)[0].set(1.0f,1.0f,1.0f,1.0f);
                    geometry->setColorArray(colours, osg::Array::BIND_OVERALL);
                }
            }
        }
    }

    virtual void apply(osg::Node& node) { traverse(node); }

};

osg::ref_ptr<osg::Node> readModel( int argc, char **argv );

#endif
