CMAKE_MINIMUM_REQUIRED( VERSION 3.1.0 FATAL_ERROR )

#To Run in debug Mode...
# Set argument
# -DCMAKE_BUILD_TYPE=Debug
# when running CMAKE

PROJECT( OSG2CityGML CXX)

SET( CMAKE_AUTOMOC ON )
SET( CMAKE_INCLUDE_CURRENT_DIR ON )
SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)
cmake_policy(SET CMP0015 NEW)

set(QT_USE_QTGUI TRUE)
set(QT_USE_QTXML TRUE)

SET( CMAKE_INCLUDE_CURRENT_DIR ON )
SET( CMAKE_AUTOMOC ON )

IF(APPLE)
    ADD_DEFINITIONS(
        -std=c++11
    )
ENDIF(APPLE)

FILE(GLOB SRC "src/*.cpp")
FILE(GLOB INC_DIR "include")
FILE(GLOB INC_FILES "include/*.h")

FIND_PACKAGE( Qt5Core )
FIND_PACKAGE( Qt5Gui  )
FIND_PACKAGE( Qt5OpenGL )
FIND_PACKAGE( Qt5Xml )
FIND_PACKAGE( Qt5XmlPatterns )
FIND_PACKAGE( OpenGL REQUIRED )
FIND_PACKAGE( OpenSceneGraph 3.4 REQUIRED COMPONENTS osgDB osgGA osgUtil osgViewer osgText)

INCLUDE_DIRECTORIES( ${OPENSCENEGRAPH_INCLUDE_DIRS} ${CMAKE_BINARY_DIR}/include include)

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

ADD_EXECUTABLE( OSG2CityGML ${SRC} ${INC_FILES} ${UI_HEADERS} ${UI_FILES} ${RESOURCE_ADDED})

IF(MSVC)
    set_property(TARGET OSG2CityGML PROPERTY CXX_STANDARD 14)
    set_property(TARGET OSG2CityGML PROPERTY CXX_STANDARD_REQUIRED ON)
ENDIF(MSVC)

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
