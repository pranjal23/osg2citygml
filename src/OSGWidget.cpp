#include "OSGWidget.h"

#include <osg/Camera>

#include <osg/DisplaySettings>
#include <osg/Material>
#include <osg/StateSet>

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

#include <osgGA/EventQueue>
#include <osgGA/TrackballManipulator>

#include <osgUtil/IntersectionVisitor>
#include <osgUtil/PolytopeIntersector>

#include <osgViewer/View>
#include <osgViewer/ViewerEventHandlers>

#include <cassert>

#include <stdexcept>
#include <vector>

#include <QDebug>
#include <QKeyEvent>
#include <QPainter>
#include <QWheelEvent>

#include "OsgConv.h"

OSGWidget::OSGWidget( QWidget* parent,
                      Qt::WindowFlags f )
    : QOpenGLWidget( parent,
                     f )
    , graphicsWindow_( new osgViewer::GraphicsWindowEmbedded( this->x(),
                                                              this->y(),
                                                              this->width(),
                                                              this->height() ) )
    , viewer_( new osgViewer::CompositeViewer )
{
    this->setFile("");

    //register for mouse events
    this->setMouseTracking( true );
}

OSGWidget::~OSGWidget()
{
}

void OSGWidget::paintEvent( QPaintEvent* /* paintEvent */ )
{
    this->makeCurrent();

    QPainter painter( this );
    painter.setRenderHint( QPainter::Antialiasing );

    this->paintGL();

    painter.end();

    this->doneCurrent();
}

void OSGWidget::paintGL()
{
    viewer_->frame();
}

void OSGWidget::resizeWidget(int width, int height){
    this->resize(width,height);
}

void OSGWidget::resizeGL( int width, int height )
{
    this->getEventQueue()->windowResize( this->x(), this->y(), width, height );
    graphicsWindow_->resized( this->x(), this->y(), width, height );

    this->onResize( width, height );
}

void OSGWidget::keyPressEvent( QKeyEvent* event )
{
    QString keyString   = event->text();
    const char* keyData = keyString.toLocal8Bit().data();

    if( event->key() == Qt::Key_H )
    {
        this->onHome();
        return;
    }

    this->getEventQueue()->keyPress( osgGA::GUIEventAdapter::KeySymbol( *keyData ) );
}

void OSGWidget::keyReleaseEvent( QKeyEvent* event )
{
    QString keyString   = event->text();
    const char* keyData = keyString.toLocal8Bit().data();

    this->getEventQueue()->keyRelease( osgGA::GUIEventAdapter::KeySymbol( *keyData ) );
}

void OSGWidget::mouseMoveEvent( QMouseEvent* event )
{
    this->getEventQueue()->mouseMotion( static_cast<float>( event->x() ),
                                        static_cast<float>( event->y() ) );
}

void OSGWidget::mousePressEvent( QMouseEvent* event )
{
    unsigned int button = 0;

    switch( event->button() )
    {
    case Qt::LeftButton:
        button = 1;
        break;

    case Qt::MiddleButton:
        button = 2;
        break;

    case Qt::RightButton:
        button = 3;
        break;

    default:
        break;
    }

    this->getEventQueue()->mouseButtonPress( static_cast<float>( event->x() ),
                                             static_cast<float>( event->y() ),
                                             button );

}

void OSGWidget::mouseReleaseEvent(QMouseEvent* event)
{
    unsigned int button = 0;

    switch( event->button() )
    {
    case Qt::LeftButton:
        button = 1;
        break;

    case Qt::MiddleButton:
        button = 2;
        break;

    case Qt::RightButton:
        button = 3;
        break;

    default:
        break;
    }

    this->getEventQueue()->mouseButtonRelease( static_cast<float>( event->x() ),
                                               static_cast<float>( event->y() ),
                                               button );
}

void OSGWidget::wheelEvent( QWheelEvent* event )
{
    event->accept();
    int delta = event->delta();

    osgGA::GUIEventAdapter::ScrollingMotion motion = delta > 0 ?   osgGA::GUIEventAdapter::SCROLL_UP
                                                                 : osgGA::GUIEventAdapter::SCROLL_DOWN;

    this->getEventQueue()->mouseScroll( motion );
}

bool OSGWidget::event( QEvent* event )
{
    bool handled = QOpenGLWidget::event( event );

    switch( event->type() )
    {
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
    case QEvent::MouseButtonDblClick:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseMove:
    case QEvent::Wheel:
        this->update(); //Repaint after an event
        break;

    default:
        break;
    }

    return handled;
}

void OSGWidget::onHome()
{
    osgViewer::ViewerBase::Views views;
    viewer_->getViews( views );

    for( std::size_t i = 0; i < views.size(); i++ )
    {
        osgViewer::View* view = views.at(i);
        view->home();
    }
}

void OSGWidget::onResize( int width, int height )
{
    std::vector<osg::Camera*> cameras;
    viewer_->getCameras( cameras );

    cameras[0]->setViewport( 0, 0, this->width(), this->height() );
}

osgGA::EventQueue* OSGWidget::getEventQueue() const
{
    osgGA::EventQueue* eventQueue = graphicsWindow_->getEventQueue();

    if( eventQueue )
        return eventQueue;
    else
        throw std::runtime_error( "Unable to obtain valid event queue");
}

void analysePrimSet(osg::PrimitiveSet*prset, const osg::Vec3Array *verts) {
    unsigned int ic;
    unsigned int i2;
    unsigned int nprim=0;
    osg::notify(osg::WARN) << "Prim set type "<< prset->getMode() << std::endl;
    for (ic=0; ic<prset->getNumIndices(); ic++) { // NB the vertices are held in the drawable -
        osg::notify(osg::WARN) << "vertex "<< ic << " is index "<<prset->index(ic) << " at " <<
            (* verts)[prset->index(ic)].x() << "," <<
            (* verts)[prset->index(ic)].y() << "," <<
            (* verts)[prset->index(ic)].z() << std::endl;
    }
    // you might want to handle each type of primset differently: such as:
    switch (prset->getMode()) {
    case osg::PrimitiveSet::TRIANGLES: // get vertices of triangle
        osg::notify(osg::WARN) << "Triangles "<< nprim << " is index "<<prset->index(ic) << std::endl;
        for (i2=0; i2<prset->getNumIndices()-2; i2+=3) {
        }
    break;
    case osg::PrimitiveSet::TRIANGLE_STRIP: // look up how tristrips are coded
    break;
    // etc for all the primitive types you expect. EG quads, quadstrips lines line loops....
    }
}

void OSGWidget::setFile(QString fileName){

    float aspectRatio = static_cast<float>( this->width() ) / static_cast<float>( this->height() );
    osg::ref_ptr<osg::Group> root = new osg::Group;

    if(!fileName.isEmpty()){

        const char* src = fileName.toUtf8().constData();

        char dest[100];
        memset(dest, '\0', sizeof(dest));
        strcpy(dest, src);

        char* args[10];
        args[0] = " ";
        args[1] = dest;
        osg::ref_ptr<osg::Node> model = readModel(2, args);

        //osg::ref_ptr<osg::Node> model= osgDB::readNodeFile(fileName.toUtf8().constData());
        if(model==NULL)
            return;

        osg::Geode *geode;
        osg::Group *grp;
        osg::Geometry *geom;

        grp = model.get()->asGroup();
        if(grp==0){
            osg::notify(osg::WARN) << "Group = 0" << std::endl;
        } else {
            osg::notify(osg::WARN) << "In Here..." << std::endl;
            int i;
            for (i = 0; i < grp->getNumChildren(); i++) {
                osg::notify(osg::WARN) << "Child - " << grp->getChild(i)->className() << std::endl;

                geode = (osg::Geode*)grp->getChild(i);

                if(geode==0){
                    osg::notify(osg::WARN) << "Geode = 0" << std::endl;
                }
                else
                {
                    int j;
                    for (j = 0; j < geode->getNumChildren(); j++) {
                        osg::notify(osg::WARN) << "Child Child - " << geode->getChild(j)->className() << std::endl;

                        geom = (osg::Geometry*)grp->getChild(i);

                        int j;
                        for (j = 0; j < geode->getNumChildren(); j++) {
                            //osg::notify(osg::WARN) << "The number of elements in vertex array- " << geom-> << std::endl;
                        }
                        /*for (unsigned int ipr=0; ipr<geom->getNumPrimitiveSets(); ipr++) {
                            osg::PrimitiveSet* prset=geom->getPrimitiveSet(ipr);
                            osg::notify(osg::WARN) << "Primitive Set "<< ipr << std::endl;
                            //analysePrimSet(prset, dynamic_cast<const osg::Vec3Array*>(geom->getVertexArray()));
                        }*/
                    }
                }
            }
        }

        //Render it...
        root->addChild(model.get());
    }
    else
    {
        osg::Geode* pyramidGeode = new osg::Geode();
        osg::Geometry* pyramidGeometry = new osg::Geometry();

        pyramidGeode->addDrawable(pyramidGeometry);
        root->addChild(pyramidGeode);

        osg::Vec3Array* pyramidVertices = new osg::Vec3Array;
        pyramidVertices->push_back( osg::Vec3( 0, 0, 0) ); // front left
        pyramidVertices->push_back( osg::Vec3(10, 0, 0) ); // front right
        pyramidVertices->push_back( osg::Vec3(10,10, 0) ); // back right
        pyramidVertices->push_back( osg::Vec3( 0,10, 0) ); // back left
        pyramidVertices->push_back( osg::Vec3( 5, 5,10) ); // peak

        pyramidGeometry->setVertexArray( pyramidVertices );

        osg::DrawElementsUInt* pyramidBase =
                new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 0);
        pyramidBase->push_back(3);
        pyramidBase->push_back(2);
        pyramidBase->push_back(1);
        pyramidBase->push_back(0);
        pyramidGeometry->addPrimitiveSet(pyramidBase);

        osg::DrawElementsUInt* pyramidFaceOne =
                new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
        pyramidFaceOne->push_back(0);
        pyramidFaceOne->push_back(1);
        pyramidFaceOne->push_back(4);
        pyramidGeometry->addPrimitiveSet(pyramidFaceOne);

        osg::DrawElementsUInt* pyramidFaceTwo =
                new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
        pyramidFaceTwo->push_back(1);
        pyramidFaceTwo->push_back(2);
        pyramidFaceTwo->push_back(4);
        pyramidGeometry->addPrimitiveSet(pyramidFaceTwo);

        osg::DrawElementsUInt* pyramidFaceThree =
                new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
        pyramidFaceThree->push_back(2);
        pyramidFaceThree->push_back(3);
        pyramidFaceThree->push_back(4);
        pyramidGeometry->addPrimitiveSet(pyramidFaceThree);

        osg::DrawElementsUInt* pyramidFaceFour =
                new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
        pyramidFaceFour->push_back(3);
        pyramidFaceFour->push_back(0);
        pyramidFaceFour->push_back(4);
        pyramidGeometry->addPrimitiveSet(pyramidFaceFour);

        osg::Vec4Array* colors = new osg::Vec4Array;
        colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f) ); //index 0 red
        colors->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f) ); //index 1 green
        colors->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f) ); //index 2 blue
        colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f) ); //index 3 white
        colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f) ); //index 4 red

        pyramidGeometry->setColorArray(colors);
        pyramidGeometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
    }


    // Set material for basic lighting and enable depth tests.
    osg::StateSet* stateSet = root.get()->getOrCreateStateSet();
    osg::Material* material = new osg::Material;

    material->setColorMode( osg::Material::AMBIENT_AND_DIFFUSE );

    stateSet->setAttributeAndModes( material, osg::StateAttribute::ON );
    stateSet->setMode( GL_DEPTH_TEST, osg::StateAttribute::ON );

    osg::Camera* camera = new osg::Camera;
    camera->setViewport( 0, 0, this->width(), this->height() );
    camera->setClearColor( osg::Vec4( 0.2f, 0.2f, 0.2f, 1.f ) );
    camera->setProjectionMatrixAsPerspective( 45.f, aspectRatio, 0.5f, 1000.f );
    camera->setGraphicsContext( graphicsWindow_ );

    osgViewer::View* view = new osgViewer::View;
    view->setCamera( camera );
    view->setSceneData( root.get() );
    view->addEventHandler( new osgViewer::StatsHandler );

    osgGA::TrackballManipulator* manipulator = new osgGA::TrackballManipulator;
    manipulator->setAllowThrow( false );

    view->setCameraManipulator( manipulator );

    viewer_->addView( view );

    viewer_->setThreadingModel( osgViewer::CompositeViewer::SingleThreaded );
    viewer_->realize();
}
