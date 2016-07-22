CMAKE_MINIMUM_REQUIRED( VERSION 2.8.11 )
PROJECT( OSG2CityGML )

SET( CMAKE_CXX_FLAGS "-std=c++11 -Wall -pedantic -Wno-long-long" )

SET( CMAKE_INCLUDE_CURRENT_DIR ON )
SET( CMAKE_AUTOMOC ON )

SET( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/;${CMAKE_MODULE_PATH}" )
SET(ENV{XERCESC_ROOT} "/usr/local/Cellar/xerces-c/3.1.2")

#To Run in debug Mode...
# Set argument
# -DCMAKE_BUILD_TYPE=Debug
# when running CMAKE

FIND_PACKAGE( Qt5Core )
FIND_PACKAGE( Qt5Gui  )
FIND_PACKAGE( Qt5OpenGL )
FIND_PACKAGE( Qt5Xml )
FIND_PACKAGE( Qt5XmlPatterns )
FIND_PACKAGE( OpenSceneGraph REQUIRED COMPONENTS osgDB osgGA osgUtil osgViewer )
FIND_PACKAGE( XercesC )

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
  include/CityGML.h
  include/SchemaValidator.h
)

FILE(
GLOB_RECURSE
UI_FILES
forms/*.ui
)

QT5_WRAP_UI(UI_HEADERS ${UI_FILES})

ADD_EXECUTABLE( OSG2CityGML ${SOURCES} ${HEADERS} ${UI_HEADERS} ${UI_FILES})

FILE(GLOB SCHEMA_BASE schema/*)

FILE(COPY ${SCHEMA_BASE} DESTINATION schema)

TARGET_LINK_LIBRARIES( OSG2CityGML
  ${OPENSCENEGRAPH_LIBRARIES}
  Qt5::Core
  Qt5::Gui
  Qt5::OpenGL
  Qt5::Xml
  Qt5::XmlPatterns
)
