#pragma once
#ifndef OSGWidget_h__
#define OSGWidget_h__

#include <QPoint>
#include <QOpenGLWidget>
#include <QException>

#include <osg/ref_ptr>

#include <osgViewer/GraphicsWindow>
#include <osgViewer/CompositeViewer>

#include <osg/Camera>

#include <osg/DisplaySettings>
#include <osg/Material>
#include <osg/StateSet>
#include <osg/CullFace>

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

#include <osgGA/EventQueue>

#include <osgUtil/IntersectionVisitor>
#include <osgUtil/PolytopeIntersector>

#include <osgViewer/View>
#include <osgViewer/ViewerEventHandlers>
#include <osg/PolygonOffset>

#include <cassert>

#include <stdexcept>
#include <vector>

#include <QDebug>
#include <QKeyEvent>
#include <QPainter>
#include <QWheelEvent>

#include "GeometryHandler.h"
#include "UIHelper.h"


class OSGWidget : public QOpenGLWidget
{
  Q_OBJECT

public:
  OSGWidget( QWidget* parent = 0,
             Qt::WindowFlags f = 0 );

  virtual ~OSGWidget();
  void setFile(QString fileName);
  void saveObject2File(QString fileName);
  void saveOSG2File(QString fileName);
  void setView();
  void resizeWidget(int width, int height);
  void convertToTrianglePrimitives(bool verbose=false);
  void renderModel();
  void setLocationBasedSegmentation(bool checked);
  bool getLocationBasedSegmentation();
  void setNormalsBasedSegmentation(bool checked);
  bool getNormalsBasedSegmentation();
  void setNormalsDistance(double value);
  bool getShapeBasedSegmentation();
  void setShapeBasedSegmentation(bool checked);
  double getNormalsDistance();
  void selectAllPolygons();
  void clearAllSelection();
  void setEditableStateSet();
  void tagSelectedItems(QString name_space, QString element_name);
  void selectElementItems(QString name_space, QString element_name, bool deselect);
  void generateMoreGraphLinks();
  void recalculateUpVector();
  void setCameraTranslateMode(bool setOn);

  osg::ref_ptr<osg::Group> getEditableModelGroup();
  PrimitiveNode& getPolygonNode(osg::Drawable* drawable, unsigned int index);
  QList<PrimitiveNode>& getAllPolygonNodes();

  bool selectMode = true;
  bool cityObjectGroup = false;
  bool showNormalGlyph = false;
  bool drawGraphEdges = false;
  bool renderEditableMode = true;
  bool renderWireFrame = false;
  bool click_n_drag=false;
  bool paintSelectionMode=false;

  float link_precision = 6;

  Shape shape_to_segment = Shape::Plain;
  osg::Vec3f* up_vector;
  osg::ref_ptr<osgViewer::CompositeViewer> viewer_;

protected:
  virtual void paintEvent( QPaintEvent* paintEvent );
  virtual void paintGL();
  virtual void resizeGL( int width, int height );

  virtual void keyPressEvent( QKeyEvent* event );
  virtual void keyReleaseEvent( QKeyEvent* event );

  virtual void mouseMoveEvent( QMouseEvent* event );
  virtual void mousePressEvent( QMouseEvent* event );
  virtual void mouseReleaseEvent( QMouseEvent* event );
  virtual void wheelEvent( QWheelEvent* event );

  virtual bool event( QEvent* event );

private:

  virtual void onHome();
  virtual void onResize( int width, int height );
  void addGlyph();

  bool locationBasedSegmentation = false;
  bool normalsBasedSegmentation = false;
  bool shapeBasedSegmentation = false;
  double normalsDistance = 0.0;


  osgGA::EventQueue* getEventQueue() const;

  osg::ref_ptr<osgViewer::GraphicsWindowEmbedded> graphicsWindow_;
  osg::ref_ptr<osg::Group> rootSceneGroup;

  osg::ref_ptr<osg::Group> editableModelGroup;
  osg::ref_ptr<osg::Group> originalModelGroup;

  osg::ref_ptr<osg::Node> glyphGroup;
  osg::ref_ptr<osgViewer::View> view;
};

#endif
