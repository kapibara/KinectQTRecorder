cmake_minimum_required(VERSION 2.8)

project(KinectQTRecorderPlot)

set(HOME ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_BUILD_TYPE Release)

set(CMAKE_INSTALL_PREFIX ${HOME}/bin)

set(KINECT_SDK_PATH "C:/Program Files/Microsoft SDKs/Kinect/v2.0-PublicPreview1408")
set(KINECT2_WRAPPER_PATH C:/Data/Development/CPP/Kinect2CameraInterface)

set(KINECT2_LIB_DIR ${KINECT2_WRAPPER_PATH}/lib)
set(KINECT2_INCLUDE_DIR ${KINECT2_WRAPPER_PATH}/include)
set(KINECT2_LIBS libkinect2 Kinect20) #first is the wrapper lib, second is SDK lib

set(LIBPNG "C:/Program Files (x86)/GnuWin32/")
set(LIBPNG_LIB_DIR ${LIBPNG}/lib)
set(LIBPNG_INCLUDE_DIR ${LIBPNG}/include)
set(LIBPNG_LIBS libpng zlibstat)

#everything else
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Multimedia REQUIRED)
#Added for plotting
find_package(Qt5OpenGL REQUIRED)
find_package(OpenGL REQUIRED)

add_definitions(${Qt5Widgets_DEFINITIONS})
add_definitions(${Qt5Multimedia_DEFINITIONS})

include_directories(${HOME}/include)
include_directories(${KINECT2_INCLUDE_DIR})
include_directories(${LIBPNG_INCLUDE_DIR})
include_directories(${KINECT_SDK_PATH}/inc)
include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${Qt5Multimedia_INCLUDE_DIRS})

link_directories(${KINECT2_LIB_DIR})
link_directories(${LIBPNG_LIB_DIR})
link_directories(${KINECT_SDK_PATH}/Lib/x86)

add_subdirectory(${HOME}/src)

