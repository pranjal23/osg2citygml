#include "OSGWidget.h"
#include "SelectionHandler.h"
#include "CityGMLWriter.h"
#include "AppCameraManipulator.h"
#include <osgGA/FirstPersonManipulator>

using namespace osg2citygml;

osg::ref_ptr<SelectionHandler> selectionHandler;

osg::ref_ptr<osg2citygml::AppCameraManipulator> acManipulator;
osg::ref_ptr<osgGA::FirstPersonManipulator> fpManipulator;

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

    if( event->key() == Qt::Key_C )
    {
        //HANDLED IN PICKING HANDLER... DO NOT CHANGE
    }

    if( event->key() == Qt::Key_Z )
    {
        selectAllPolygons();
    }

    this->getEventQueue()->keyPress( osgGA::GUIEventAdapter::KeySymbol( *keyData ) );
}

void OSGWidget::keyReleaseEvent( QKeyEvent* event )
{
    QString keyString   = event->text();
    const char* keyData = keyString.toLocal8Bit().data();

    if( event->key() == Qt::Key_Control )
    {
        paintSelectionMode = false;
    }

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

void OSGWidget::setShapeBasedSegmentation(bool checked)
{
    shapeBasedSegmentation = checked;
}

bool OSGWidget::getShapeBasedSegmentation()
{
    return shapeBasedSegmentation;
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

QList<PrimitiveNode>& OSGWidget::getAllPolygonNodes()
{
    QList<PrimitiveNode>* list = new QList<PrimitiveNode>();

    int i;
    for (i = 0; i < editableModelGroup.get()->getNumChildren(); i++)
    {
        osg::Geode* geode = (osg::Geode*)editableModelGroup.get()->getChild(i);
        for(unsigned int i=0; i<geode->getNumDrawables(); i++)
        {
            osg::Geometry* geometry = dynamic_cast<osg::Geometry*>(geode->getDrawable(i));

            GraphData* userData = dynamic_cast<GraphData*>(geometry->getUserData());

            for(std::map<unsigned int,PrimitiveNode>::iterator it = userData->primitivesMap->begin();it!=userData->primitivesMap->end();it++)
            {
                list->push_back(it->second);
            }
        }
    }

    return *list;
}

void OSGWidget::selectAllPolygons()
{
    QList<PrimitiveNode> list;

    int i;
    for (i = 0; i < editableModelGroup.get()->getNumChildren(); i++)
    {
        osg::Geode* geode = (osg::Geode*)editableModelGroup.get()->getChild(i);
        for(unsigned int i=0; i<geode->getNumDrawables(); i++)
        {
            osg::Geometry* geometry = dynamic_cast<osg::Geometry*>(geode->getDrawable(i));

            GraphData* userData = dynamic_cast<GraphData*>(geometry->getUserData());

            for(std::map<unsigned int,PrimitiveNode>::iterator it = userData->primitivesMap->begin();it!=userData->primitivesMap->end();it++)
            {
                list.push_back(it->second);
            }
        }
    }

    selectionHandler.get()->addToSelectedPrimitiveList(list,false);
    selectionHandler.get()->colorify();
}

void OSGWidget::clearAllSelection()
{
    selectionHandler.get()->clearSelectedList();
    selectionHandler.get()->colorify();
}

void OSGWidget::selectElementItems(QString name_space, QString element_name, bool deselect )
{
    if(!editableModelGroup.get())
        return;

    QList<PrimitiveNode> list;

    unsigned int k;
    for (k = 0; k < editableModelGroup.get()->getNumChildren(); k++)
    {
        osg::Geode* geode = (osg::Geode*)editableModelGroup.get()->getChild(k);
        unsigned int i;
        for(i=0; i < geode->getNumDrawables(); i++)
        {
            osg::Geometry* geometry = dynamic_cast<osg::Geometry*>(geode->getDrawable(i));

            GraphData* userData = dynamic_cast<GraphData*>(geometry->getUserData());

            for(std::map<unsigned int,PrimitiveNode>::iterator it = userData->primitivesMap->begin();it!=userData->primitivesMap->end();it++)
            {
                PrimitiveNode a =(*it).second;
                if(a.element_name == element_name
                        && a.name_space == name_space)
                {
                    list.push_back(a);
                }
            }
        }
    }

    selectionHandler.get()->addToSelectedPrimitiveList(list,deselect);
    selectionHandler.get()->colorify();

    QString selected = "Selected ";
    if(deselect)
        selected = "Deselected ";

    QMessageBox* msg = new QMessageBox();
    msg->setText(selected  + QString::number(list.size()) + " polygons!!!" );
    msg->show();
}

void OSGWidget::tagSelectedItems(QString name_space, QString element_name)
{
    std::multimap<unsigned int,PrimitiveNode>* selectedPrimitives =
            selectionHandler.get()->selectedPrimitives;

    if(selectedPrimitives->size()<=0)
        return;

    for(std::multimap<unsigned int,PrimitiveNode>::iterator itS = selectedPrimitives->begin();itS!=selectedPrimitives->end();itS++)
    {
        PrimitiveNode s = (*itS).second;

        unsigned int i;
        for (i = 0; i < editableModelGroup.get()->getNumChildren(); i++)
        {
            osg::Geode* geode = (osg::Geode*)editableModelGroup.get()->getChild(i);
            for(unsigned int i=0;i<geode->getNumDrawables();i++)
            {
                if(s.drawable != geode->getDrawable(i))
                    continue;

                osg::Geometry* geometry = dynamic_cast<osg::Geometry*>(geode->getDrawable(i));
                GraphData* userData = dynamic_cast<GraphData*>(geometry->getUserData());

                PrimitiveNode p = (*(userData->primitivesMap))[s.primitiveIndex];
                p.element_name = element_name;
                p.name_space = name_space;
                (*(userData->primitivesMap))[s.primitiveIndex] = p;
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
                GraphData* userData = dynamic_cast<GraphData*>(geometry->getUserData());

                for(std::map<unsigned int,PrimitiveNode>::iterator it = userData->primitivesMap->begin();it!=userData->primitivesMap->end();it++)
                {
                    PrimitiveNode a = it->second;
                    qDebug() << a.name_space << ":" << a.element_name << ", Links: " ;

                    for(unsigned int k =0; k<a.links->size();k++)
                    {
                        qDebug() << QString::number(a.links->at(k).primitiveIndex);
                    }

                }
            }

        }
    }
}

PrimitiveNode& OSGWidget::getPolygonNode(osg::Drawable* drawable, unsigned int index)
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
            GraphData* userData = dynamic_cast<GraphData*>(geometry->getUserData());

            return (*(userData->primitivesMap))[index];
        }
    }

    qDebug() << "No Polygon Node found, throwing exception!!!";
    throw new QException();
}

void OSGWidget::convertToTrianglePrimitives(bool verbose){
    TrianglePrimitivesConverter triangleConverter;
    triangleConverter.setVerbose(verbose);
    triangleConverter.apply(originalModelGroup.get());

}

void OSGWidget::recalculateUpVector()
{
    std::multimap<unsigned int,PrimitiveNode>* selectedPrimitives =
            selectionHandler.get()->selectedPrimitives;

    if(selectedPrimitives->size()<=0 || selectedPrimitives->size()>1)
    {
        QMessageBox msgBox;
        msgBox.setText("Please select only one polygon whose normal will be used as up vector!!!");
        msgBox.exec();
        return;
    }

    for(std::multimap<unsigned int,PrimitiveNode>::iterator itS = selectedPrimitives->begin();itS!=selectedPrimitives->end();itS++)
    {
        PrimitiveNode s = (*itS).second;
        up_vector = new osg::Vec3f(s.faceNormal->x(),s.faceNormal->y(),s.faceNormal->z());
    }
}

void OSGWidget::renderModel()
{
    if(editableModelGroup.get()==nullptr)
            return;

    if(renderEditableMode)
    {
        rootSceneGroup->removeChildren(0,rootSceneGroup->getNumChildren());

        setEditableStateSet();

        //Editable Model Render
        rootSceneGroup->addChild(editableModelGroup.get());
    }
    else
    {
        rootSceneGroup->removeChildren(0,rootSceneGroup->getNumChildren());

        //Original Model Render
        rootSceneGroup->addChild(originalModelGroup.get());

    }

    addGlyph();
}

void OSGWidget::setEditableStateSet()
{
        // set up the state so that the underlying color is not seen through
        // and that the drawing mode is changed to wireframe, and a polygon offset
        // is added to ensure that we see the wireframe itself, and turn off
        // so texturing too.
        osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
        osg::ref_ptr<osg::PolygonOffset> polyoffset = new osg::PolygonOffset;
        polyoffset->setFactor(-1.0f);
        polyoffset->setUnits(-1.0f);
        osg::ref_ptr<osg::PolygonMode> polymode = new osg::PolygonMode;
        polymode->setMode(osg::PolygonMode::FRONT_AND_BACK,osg::PolygonMode::LINE);

        if(renderWireFrame)
        {
            stateset->setAttributeAndModes(polyoffset,osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON);
            stateset->setAttributeAndModes(polymode,osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON);
        }
        else
        {
            stateset->setAttributeAndModes(polyoffset,osg::StateAttribute::INHERIT|osg::StateAttribute::OFF);
            stateset->setAttributeAndModes(polymode,osg::StateAttribute::INHERIT|osg::StateAttribute::OFF);
        }

        stateset->setTextureMode(0,GL_TEXTURE_2D,osg::StateAttribute::OVERRIDE|osg::StateAttribute::OFF);


        editableModelGroup.get()->setStateSet(stateset);
}

void OSGWidget::setFile(QString fileName){

    rootSceneGroup = new osg::Group;

    if(!fileName.isEmpty())
    {
        osg::ref_ptr<osg::Node> node;
        node = osgDB::readNodeFile(fileName.toStdString());
        if (node == nullptr) {
            std::cerr << "Failed to load file " << fileName.toStdString() << std::endl;
            return;
        }

        osg::Group *origGroup = node->asGroup();

        if(origGroup==0){
            osg::notify(osg::WARN) << "Group = 0" << std::endl;
        } else {
            //osg::notify(osg::WARN) << "Number of Children in group - " << origGroup->getNumChildren() << std::endl;

            originalModelGroup = origGroup;

            convertToTrianglePrimitives();

            editableModelGroup = new osg::Group(*(originalModelGroup.get()),osg::CopyOp::DEEP_COPY_ALL);

            GraphGenerator grapGen;
            grapGen.generate(editableModelGroup.get());
        }
    }

    setView();
    renderModel();
    this->onHome();
}

void OSGWidget::generateMoreGraphLinks()
{
    PrimitiveGraphEnhancer pGE;
    pGE.generateTestLinksFromNearbyVertices(editableModelGroup.get(), link_precision);
}

void OSGWidget::saveObject2File(QString fileName)
{
    CityGMLWriter writer(fileName, this);
    writer.write(editableModelGroup.get());
}

void OSGWidget::saveOSG2File(QString fileName)
{
    osgDB::writeNodeFile(*(editableModelGroup.get()->asNode()), fileName.toStdString());
}

void OSGWidget::setCameraTranslateMode(bool setOn)
{
    if(setOn)
    {
        osg::Vec3d eye;
        osg::Vec3d center;
        osg::Vec3d up;
        acManipulator.get()->getTransformation(eye,center,up);
        fpManipulator.get()->setTransformation(eye,center,up);
        view.get()->setCameraManipulator( fpManipulator.get(), false);
    }
    else
    {
        //osg::Vec3d eye;
        //osg::Vec3d center;
        //osg::Vec3d up;
        //fpManipulator.get()->getTransformation(eye,center,up);
        //acManipulator.get()->setHomePosition(eye,center,up);
        view.get()->setCameraManipulator( acManipulator.get(), false);
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

    stateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
    stateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
    stateSet->setRenderBinDetails(1, "DepthSortedBin");

    osg::CullFace* cull = new osg::CullFace();
    cull->setMode(osg::CullFace::BACK);
    stateSet->setAttributeAndModes(cull, osg::StateAttribute::ON);

    osg::Camera* camera = new osg::Camera;
    camera->setViewport( 0, 0, this->width(), this->height() );
    camera->setClearColor( osg::Vec4( 0.2f, 0.2f, 0.2f, 1.f ) );
    camera->setProjectionMatrixAsPerspective( 45.f, aspectRatio, 0.5f, 1000.f );
    camera->setGraphicsContext( graphicsWindow_ );

    //osgGA::TrackballManipulator* manipulator = new osgGA::TrackballManipulator();
    acManipulator = new osg2citygml::AppCameraManipulator(osg2citygml::AppCameraManipulator::DEFAULT_SETTINGS,this);
    acManipulator->setAllowThrow(false);

    fpManipulator = new osgGA::FirstPersonManipulator();
    fpManipulator->setAllowThrow(false);

    selectionHandler = new SelectionHandler(this);

    view = new osgViewer::View;
    view.get()->setCamera( camera );
    view.get()->setSceneData( rootSceneGroup.get() );
    //view->addEventHandler( new osgViewer::StatsHandler );

    view.get()->setCameraManipulator( acManipulator.get() );
    view.get()->addEventHandler(selectionHandler.get() );

    viewer_->addView( view );
    viewer_->setThreadingModel( osgViewer::CompositeViewer::SingleThreaded );
    viewer_->realize();
}

void OSGWidget::addGlyph()
{

    if(showNormalGlyph)
    {
        GlyphHelper gen;
        osg::Geode* glyphGeode = new osg::Geode();
        glyphGeode->addDrawable(gen.getNormalsGlyphGeometry(getAllPolygonNodes()));
        rootSceneGroup.get()->addChild(glyphGeode->asGroup());
    }

    if(drawGraphEdges)
    {
        GlyphHelper gen;
        osg::Geode* edgesGeode = new osg::Geode();
        edgesGeode->addDrawable(gen.getPolygonsGlyphGeometry(getAllPolygonNodes()));
        rootSceneGroup.get()->addChild(edgesGeode->asGroup());
    }
}
