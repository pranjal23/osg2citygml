CMAKE_MINIMUM_REQUIRED( VERSION 3.1.0 FATAL_ERROR )

#To Run in debug Mode...
# Set argument
# -DCMAKE_BUILD_TYPE=Debug
# when running CMAKE

PROJECT( OSG2CityGML CXX)

set(QT_USE_QTGUI TRUE)
set(QT_USE_QTXML TRUE)

SET( CMAKE_INCLUDE_CURRENT_DIR ON )
SET( CMAKE_AUTOMOC ON )

IF(APPLE)
    ADD_DEFINITIONS(
        -std=c++11
    )
ELSE(APPLE)
    set_property(TARGET OSG2CityGML PROPERTY CXX_STANDARD 11)
    set_property(TARGET OSG2CityGML PROPERTY CXX_STANDARD_REQUIRED ON)
ENDIF(APPLE)

FIND_PACKAGE( Qt5Core )
FIND_PACKAGE( Qt5Gui  )
FIND_PACKAGE( Qt5OpenGL )
FIND_PACKAGE( Qt5Xml )
FIND_PACKAGE( Qt5XmlPatterns )
FIND_PACKAGE( OpenGL REQUIRED )
FIND_PACKAGE( OpenSceneGraph REQUIRED COMPONENTS osgDB osgGA osgUtil osgViewer osgText)

INCLUDE_DIRECTORIES( ${OPENSCENEGRAPH_INCLUDE_DIRS} ${CITYGML_SOURCES_PATH}/include ${CMAKE_BINARY_DIR}/include include)

SET( SOURCES
  src/MainWindow.cpp
  src/OSGWidget.cpp
  src/OSG2CityGML.cpp
  src/CityGMLWriter.cpp
  src/UIHelperSingleton.cpp
  src/AppCameraManipulator.cpp
)

SET( HEADERS
  include/CityGMLHelper.h
  include/CityGMLWriter.h
  include/GMLHelper.h
  include/MainWindow.h
  include/OSGWidget.h
  include/GeometryHandler.h
  include/SelectionHandler.h
  include/SchemaValidator.h
  include/UIHelper.h
  include/AppCameraManipulator.h
)

FILE(
GLOB_RECURSE
UI_FILES
forms/*.ui
)

set(RESOURCE
    resources/resources.qrc
)

QT5_WRAP_UI(UI_HEADERS ${UI_FILES})

QT5_ADD_RESOURCES(RESOURCE_ADDED ${RESOURCE})

ADD_EXECUTABLE( OSG2CityGML ${SOURCES} ${HEADERS} ${UI_HEADERS} ${UI_FILES} ${RESOURCE_ADDED})

FILE(GLOB SCHEMA_BASE schema/*)

FILE(COPY ${SCHEMA_BASE} DESTINATION schema)
FILE(COPY ${RESOURCES_BASE} DESTINATION resources)

TARGET_LINK_LIBRARIES( OSG2CityGML
  ${OPENSCENEGRAPH_LIBRARIES}
  Qt5::Core
  Qt5::Gui
  Qt5::OpenGL
  Qt5::Xml
  Qt5::XmlPatterns
)
