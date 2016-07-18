CMAKE_MINIMUM_REQUIRED( VERSION 2.8.11 )
PROJECT( OSG2CityGML )

SET( CMAKE_CXX_FLAGS "-std=c++11 -Wall -pedantic -Wno-long-long" )

SET( CMAKE_INCLUDE_CURRENT_DIR ON )
SET( CMAKE_AUTOMOC ON )

#To Run in debug Mode...
# Set argument
# -DCMAKE_BUILD_TYPE=Debug
# when running CMAKE

FIND_PACKAGE( Qt5Core )
FIND_PACKAGE( Qt5Gui  )
FIND_PACKAGE( Qt5OpenGL )
FIND_PACKAGE( OpenSceneGraph REQUIRED COMPONENTS osgDB osgGA osgUtil osgViewer )

INCLUDE_DIRECTORIES( ${OPENSCENEGRAPH_INCLUDE_DIRS} include schema )

SET( SOURCES
  src/MainWindow.cpp
  src/OSGWidget.cpp
  src/OSG2CityGML.cpp
  src/OrientationConverter.cpp
  src/OSGConv.cpp
  src/CityGMLWriter.cpp
)

SET( HEADERS
  include/MainWindow.h
  include/OSGWidget.h
  include/OrientationConverter.h
  include/OSGConv.h
  include/OSGHelpers.h
  include/RayCastHelpers.h
  include/CityGMLWriter.h
  include/CityGMLHelper.h
)

FILE(
GLOB_RECURSE
UI_FILES
forms/*.ui
)

QT5_WRAP_UI(UI_HEADERS ${UI_FILES})

ADD_EXECUTABLE( OSG2CityGML ${SOURCES} ${HEADERS} ${UI_HEADERS} ${UI_FILES})

configure_file(schema/CityGML.xsd schema/CityGML.xsd COPYONLY)
configure_file(schema/CityGML/appearance.xsd schema/CityGML/appearance.xsd COPYONLY)
configure_file(schema/CityGML/bridge.xsd schema/CityGML/bridge.xsd COPYONLY)
configure_file(schema/CityGML/building.xsd schema/CityGML/building.xsd COPYONLY)
configure_file(schema/CityGML/cityFurniture.xsd schema/CityGML/cityFurniture.xsd COPYONLY)
configure_file(schema/CityGML/cityGMLBase.xsd schema/CityGML/cityGMLBase.xsd COPYONLY)
configure_file(schema/CityGML/cityObjectGroup.xsd schema/CityGML/cityObjectGroup.xsd COPYONLY)
configure_file(schema/CityGML/generics.xsd schema/CityGML/generics.xsd COPYONLY)
configure_file(schema/CityGML/landUse.xsd schema/CityGML/landUse.xsd COPYONLY)
configure_file(schema/CityGML/relief.xsd schema/CityGML/relief.xsd COPYONLY)
configure_file(schema/CityGML/texturedSurface.xsd schema/CityGML/texturedSurface.xsd COPYONLY)
configure_file(schema/CityGML/transportation.xsd schema/CityGML/transportation.xsd COPYONLY)
configure_file(schema/CityGML/tunnel.xsd schema/CityGML/tunnel.xsd COPYONLY)
configure_file(schema/CityGML/vegetation.xsd schema/CityGML/vegetation.xsd COPYONLY)
configure_file(schema/CityGML/waterBody.xsd schema/CityGML/waterBody.xsd COPYONLY)

TARGET_LINK_LIBRARIES( OSG2CityGML
  ${OPENSCENEGRAPH_LIBRARIES}
  Qt5::Core
  Qt5::Gui
  Qt5::OpenGL
)
