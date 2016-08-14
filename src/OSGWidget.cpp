#include "OSGWidget.h"
#include "RayCastHelpers.h"

osg::ref_ptr<PickingHandler> pickingHandler;

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

    if( event->key() == Qt::Key_C )
    {
        //HANDLED IN PICKING HANDLER... DO NOT CHANGE
    }

    if( event->key() == Qt::Key_Z )
    {
        selectAllPrimitives();
    }

    this->getEventQueue()->keyPress( osgGA::GUIEventAdapter::KeySymbol( *keyData ) );
}

void OSGWidget::selectAllPrimitives()
{
    QList<TrianglePrimitive> list;

    int i;
    for (i = 0; i < editableModelGroup.get()->getNumChildren(); i++)
    {
        osg::Geode* geode = (osg::Geode*)editableModelGroup.get()->getChild(i);
        for(unsigned int i=0;i<geode->getNumDrawables();i++)
        {
            osg::Geometry* geometry = dynamic_cast<osg::Geometry*>(geode->getDrawable(i));

            UserData* userData = dynamic_cast<UserData*>(geometry->getUserData());

            for(std::multimap<unsigned int,TrianglePrimitive>::iterator it = userData->allPrimitivesMap->begin();it!=userData->allPrimitivesMap->end();it++)
            {
                list.push_back(it->second);
            }
        }
    }

    pickingHandler.get()->clearSelectedList();
    pickingHandler.get()->addToSelectedPrimitiveList(list);
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

void OSGWidget::setLocationBasedSegmentation(bool checked)
{
    locationBasedSegmentation = checked;
}

bool OSGWidget::getLocationBasedSegmentation()
{
    return locationBasedSegmentation;
}

void OSGWidget::setNormalsBasedSegmentation(bool checked)
{
    normalsBasedSegmentation = checked;
}

bool OSGWidget::getNormalsBasedSegmentation()
{
    return normalsBasedSegmentation;
}

void OSGWidget::setNormalsDistance(double value)
{
    normalsDistance = value;
}

double OSGWidget::getNormalsDistance()
{
    return normalsDistance;
}

osg::ref_ptr<osg::Group> OSGWidget::getEditableModelGroup()
{
    return editableModelGroup;
}

void OSGWidget::tagSelectedItems(QString name_space, QString element_name)
{
    std::multimap<unsigned int,TrianglePrimitive>* selectedPrimitives =
            pickingHandler.get()->selectedPrimitives;

    if(selectedPrimitives->size()<=0)
        return;

    for(std::multimap<unsigned int,TrianglePrimitive>::iterator itS = selectedPrimitives->begin();itS!=selectedPrimitives->end();itS++)
    {
        TrianglePrimitive s = (*itS).second;

        unsigned int i;
        for (i = 0; i < editableModelGroup.get()->getNumChildren(); i++)
        {
            osg::Geode* geode = (osg::Geode*)editableModelGroup.get()->getChild(i);
            for(unsigned int i=0;i<geode->getNumDrawables();i++)
            {
                if(s.drawable != geode->getDrawable(i))
                    continue;

                osg::Geometry* geometry = dynamic_cast<osg::Geometry*>(geode->getDrawable(i));
                UserData* userData = dynamic_cast<UserData*>(geometry->getUserData());

                std::pair <std::multimap<unsigned int,TrianglePrimitive>::iterator, std::multimap<unsigned int,TrianglePrimitive>::iterator> ret;
                ret = userData->allPrimitivesMap->equal_range(s.primitiveIndex);
                for (std::multimap<unsigned int,TrianglePrimitive>::iterator it=ret.first; it!=ret.second; ++it)
                {
                    s.element_name = element_name;
                    s.name_space = name_space;
                    (*it).second = s;
                }
            }
        }
    }

    if(false)
    {
        //Test... print out all the element names
        unsigned int i;
        for (i = 0; i < editableModelGroup.get()->getNumChildren(); i++)
        {
            osg::Geode* geode = (osg::Geode*)editableModelGroup.get()->getChild(i);
            for(unsigned int i=0;i<geode->getNumDrawables();i++)
            {

                osg::Geometry* geometry = dynamic_cast<osg::Geometry*>(geode->getDrawable(i));
                UserData* userData = dynamic_cast<UserData*>(geometry->getUserData());

                for(std::multimap<unsigned int,TrianglePrimitive>::iterator it = userData->allPrimitivesMap->begin();it!=userData->allPrimitivesMap->end();it++)
                {
                    TrianglePrimitive a = it->second;
                    qDebug() << a.name_space + ":" + a.element_name ;
                }
            }

        }
    }
}

TrianglePrimitive& OSGWidget::getPrimitive(osg::Drawable* drawable,
                                           unsigned int index)
{
    unsigned int i;
    for (i = 0; i < editableModelGroup.get()->getNumChildren(); i++)
    {
        osg::Geode* geode = (osg::Geode*)editableModelGroup.get()->getChild(i);
        for(unsigned int i=0;i<geode->getNumDrawables();i++)
        {
            if(drawable!=geode->getDrawable(i))
                continue;

            osg::Geometry* geometry = dynamic_cast<osg::Geometry*>(geode->getDrawable(i));
            UserData* userData = dynamic_cast<UserData*>(geometry->getUserData());

            std::pair <std::multimap<unsigned int,TrianglePrimitive>::iterator, std::multimap<unsigned int,TrianglePrimitive>::iterator> ret;
            ret = userData->allPrimitivesMap->equal_range(index);
            for (std::multimap<unsigned int,TrianglePrimitive>::iterator it=ret.first; it!=ret.second; ++it)
            {
                return it->second;
            }
        }

    }

    throw new QException();
}


void OSGWidget::convertToTrianglePrimitives(bool verbose){
    ConvertToTrianglePrimitives triangleConverter;

    int i;
    for (i = 0; i < editableModelGroup.get()->getNumChildren(); i++)
    {
        osg::Geode* geode = (osg::Geode*)editableModelGroup.get()->getChild(i);

        triangleConverter.setVerbose(verbose);
        triangleConverter.apply(geode);
    }
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
        osg::ref_ptr<osgDB::Options> options = new osgDB::Options("usemaxlodonly storegeomids");

        QMessageBox* msg = new QMessageBox();
        msg->setText( "Loading..." );
        msg->show();
        std::cout << "Loading file: " << fileName.toStdString() << std::endl;

        osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(fileName.toStdString(), options);
        if (node == nullptr) {
            std::cerr << "Failed to load file " << fileName.toStdString() << std::endl;
            return;
        }
        originalModelGroup = node;
        /*
        originalModelGroup = osgDB::readRefNodeFile(fileName.toStdString());

        if(originalModelGroup==NULL)
        {
            return;
        }
        */

        const osg::Group *origGroup = originalModelGroup.get()->asGroup();

        if(origGroup==0){
            osg::notify(osg::WARN) << "Group = 0" << std::endl;
        } else {
            osg::notify(osg::WARN) << "Number of Children in group - " << origGroup->getNumChildren() << std::endl;

            editableModelGroup = new osg::Group(*origGroup,osg::CopyOp::DEEP_COPY_ALL);
            if(!fileName.endsWith(".gml"))
            {
                convertToTrianglePrimitives(false);
            }
            rootSceneGroup->addChild(editableModelGroup.get());
        }
    }

    setView();
}

void OSGWidget::saveObject2File(QString fileName)
{
    CityGMLWriter writer(fileName);
    writer.write(editableModelGroup.get());
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

    pickingHandler = new PickingHandler(this);
    osgViewer::View* view = new osgViewer::View;
    view->setCamera( camera );
    view->setSceneData( rootSceneGroup.get() );
    view->addEventHandler( new osgViewer::StatsHandler );
    view->addEventHandler(pickingHandler.get() );

    osgGA::TrackballManipulator* manipulator = new osgGA::TrackballManipulator;
    manipulator->setAllowThrow( false );

    view->setCameraManipulator( manipulator );

    viewer_->addView( view );
    viewer_->setThreadingModel( osgViewer::CompositeViewer::SingleThreaded );
    viewer_->realize();
}
