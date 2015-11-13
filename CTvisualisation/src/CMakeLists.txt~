cmake_minimum_required(VERSION 2.6)

# This project is designed to be built outside the Insight source tree.
PROJECT(CTvisualisation_exe)

LINK_DIRECTORIES(/usr/local/lib)
#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# Find ITK.
FIND_PACKAGE(ITK REQUIRED)
IF(ITK_FOUND)
  INCLUDE_DIRECTORIES(${ITK_INCLUDE_DIRS})
ENDIF(ITK_FOUND)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/headers)

#Find OpenGL
FIND_PACKAGE(GLUT)
IF(GLUT_FOUND)
  INCLUDE_DIRECTORIES(${GLUT_INCLUDE_DIRS})
ENDIF(GLUT_FOUND)

FIND_PACKAGE(OpenGL)
IF(OpenGL_FOUND)
  INCLUDE_DIRECTORIES(${OpenGL_INCLUDE_DIRS})
ENDIF(OpenGL_FOUND)

ADD_SUBDIRECTORY(Segmentation_Methods)
ADD_SUBDIRECTORY(Histogram_Methods)
ADD_SUBDIRECTORY(Menus)
ADD_SUBDIRECTORY(ITK_Methods)
ADD_EXECUTABLE(CTvisualisation visualisation.cxx global.cxx viewSegmentedData.cxx)

SET(requiredLibs ${requiredLibs} mgl)
TARGET_LINK_LIBRARIES(CTvisualisation HEADER_FILES ITK_Methods Menus Histogram_Methods Segmentation_Methods ${ITK_LIBRARIES} ${MATHGL_LIBRARIES} ${GLUT_LIBRARIES} ${OPENGL_LIBRARIES} ${requiredLibs} ${optionalLibs})
