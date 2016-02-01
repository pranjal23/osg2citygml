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
    setFile("");

    //register for keyevents
    this->setFocusPolicy( Qt::StrongFocus );
    this->setMinimumSize( 100, 100 );

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

void OSGWidget::setFile(QString fileName){

    float aspectRatio = static_cast<float>( this->width() ) / static_cast<float>( this->height() );
    osg::ref_ptr<osg::Group> root = new osg::Group;

    if(!fileName.isEmpty()){
        osg::ref_ptr<osg::Node> model= osgDB::readNodeFile(fileName.toUtf8().constData());
        root->addChild( model.get() );

        // Set material for basic lighting and enable depth tests.
        osg::StateSet* stateSet = root.get()->getOrCreateStateSet();
        osg::Material* material = new osg::Material;

        material->setColorMode( osg::Material::AMBIENT_AND_DIFFUSE );

        stateSet->setAttributeAndModes( material, osg::StateAttribute::ON );
        stateSet->setMode( GL_DEPTH_TEST, osg::StateAttribute::ON );

    }

    osg::Camera* camera = new osg::Camera;
    camera->setViewport( 0, 0, this->width(), this->height() );
    camera->setClearColor( osg::Vec4( 0.2f, 0.2f, 1.f, 0.5f ) );
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
