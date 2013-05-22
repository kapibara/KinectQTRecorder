cmake_minimum_required(VERSION 2.8)

project(KinectQTRecorders)

set(HOME ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_BUILD_TYPE Release)

set(CMAKE_INSTALL_PREFIX ${HOME}/bin)

set(OpenCV_DIR C:/openCV/custom_build/)

set(CMAKE_PREFIX_PATH "C:\\Qt\\Qt5.0.2\\5.0.2\\msvc2010\\")

find_package(OpenCV REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Multimedia REQUIRED)

add_definitions(${Qt5Widgets_DEFINITIONS})
add_definitions(${Qt5Multimedia_DEFINITIONS})

set(OPENNI_INCLUDE "C:/Program Files (x86)/OpenNI/Include/")
set(OPENNI_LIBS "C:/Program Files (x86)/OpenNI/Lib")

include_directories(${HOME}/include)
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${Qt5Multimedia_INCLUDE_DIRS})
include_directories(${OPENNI_INCLUDE})

link_directories(${OpenCV_INCLUDE_DIRS})
link_directories(${OPENNI_LIBS})

add_subdirectory(${HOME}/src)
