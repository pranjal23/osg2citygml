#include "OSGWidget.h"
#include "OSGConv.h"
#include "OSGHelpers.h"
#include "RayCastHelpers.h"

RayTriangleIntersectionHelper* rsih;

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
    rootSceneGroup = new osg::Group;
    this->setView();

    //register for mouse events
    this->setMouseTracking( true );

    rsih = new RayTriangleIntersectionHelper();
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

    if( event->key() == Qt::Key_Q )
    {
        this->renderOriginal();
        return;
    }

    if( event->key() == Qt::Key_W )
    {
        this->renderEditable();
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
    qDebug() << " In Mouse Button Click ";
    unsigned int button = 0;

    switch( event->button() )
    {
    case Qt::LeftButton:
        button = 1;
        rayCastClick(event);
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


void OSGWidget::rayCastClick(QMouseEvent* event)
{
    qDebug() << " In Raycastclick ";

    if(rootSceneGroup.get()->getNumChildren()==0)
        return;

    float x = (2.0f * event->x()) / this->width() - 1.0f;
    float y = 1.0f - (2.0f * event->y()) / this->height();
    float z = 1.0f;

    osg::Vec3* direction = new osg::Vec3(x,y,z);

    std::vector<osg::Camera*> cameras;
    viewer_->getCameras( cameras );

    const osg::Vec3 o = osg::Vec3(0.0f,0.0f,0.0f) *
    osg::Matrixd::inverse(cameras[0]->getViewMatrix());

    osg::Vec3* origin = new osg::Vec3(o.x(),o.y(),o.z());

    rsih->cast(*(editableModelGroup.get()),*origin,*direction);

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
        this->update();
        break;
    case QEvent::KeyRelease:
        this->update();
        break;
    case QEvent::MouseButtonDblClick:
        this->update();
        break;
    case QEvent::MouseButtonPress:
        this->update();
        break;
    case QEvent::MouseButtonRelease:
        this->update();
        break;
    case QEvent::MouseMove:
        this->update();
        break;
    case QEvent::Wheel:
        this->update();
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

void OSGWidget::addColor(){

    AddEditColoursToGeometryVisitor colorVistor;

    int i;
    for (i = 0; i < editableModelGroup.get()->getNumChildren(); i++)
    {
        osg::Geode* geode = (osg::Geode*)editableModelGroup.get()->getChild(i);
        colorVistor.apply(*geode);
    }

}

void OSGWidget::convertToTrianglePrimitives(){
    ConvertToTrianglePrimitives triangleConverter;

    int i;
    for (i = 0; i < editableModelGroup.get()->getNumChildren(); i++)
    {
        osg::Geode* geode = (osg::Geode*)editableModelGroup.get()->getChild(i);

        triangleConverter.setVerbose(false);
        triangleConverter.apply(*geode);
    }
}

void OSGWidget::setView(){
    float aspectRatio = static_cast<float>( this->width() ) / static_cast<float>( this->height() );

    // Set material for basic lighting and enable depth tests.
    osg::StateSet* stateSet = rootSceneGroup.get()->getOrCreateStateSet();
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
    view->setSceneData( rootSceneGroup.get() );
    view->addEventHandler( new osgViewer::StatsHandler );

    osgGA::TrackballManipulator* manipulator = new osgGA::TrackballManipulator;
    manipulator->setAllowThrow( false );

    view->setCameraManipulator( manipulator );

    viewer_->addView( view );

    viewer_->setThreadingModel( osgViewer::CompositeViewer::SingleThreaded );
    viewer_->realize();
}

void OSGWidget::renderOriginal()
{
    rootSceneGroup->removeChild(0,1);

    //Plain OSG Render
    rootSceneGroup->addChild(originalModelGroup.get());
}

void OSGWidget::renderEditable()
{
    rootSceneGroup->removeChild(0,1);

    //Editable OSG Render
    rootSceneGroup->addChild(editableModelGroup.get());
}

void OSGWidget::setFile(QString fileName){

    rootSceneGroup = new osg::Group;

    if(!fileName.isEmpty())
    {

        const char* src = fileName.toUtf8().constData();

        char dest[100];
        memset(dest, '\0', sizeof(dest));
        strcpy(dest, src);

        char* args[10];
        args[0] = " ";
        args[1] = dest;
        originalModelGroup = readModel(2, args);

        if(originalModelGroup==NULL)
        {
            return;
        }

        const osg::Group *origGroup = originalModelGroup.get()->asGroup();

        if(origGroup==0){
            osg::notify(osg::WARN) << "Group = 0" << std::endl;
        } else {
            osg::notify(osg::WARN) << "Number of Children in group - " << origGroup->getNumChildren() << std::endl;

            editableModelGroup = new osg::Group(*origGroup,osg::CopyOp::DEEP_COPY_ALL);
            convertToTrianglePrimitives();
            addColor();
            rootSceneGroup->addChild(editableModelGroup.get());
        }
    }

    setView();
}
