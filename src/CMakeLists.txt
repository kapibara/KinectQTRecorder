set(EXEC recorderplot)
set(RESOURCES Bad.png
              Ok.png
              boing.wav
)

set(INCLUDE_DIR ${HOME}/include)

set(SOURCE main.cpp
           qmainwindow.cpp
           qvideowidget.cpp
           qvideosurface.cpp
           #kinectmotor.cpp
           qvideosource.cpp
           qsettingswindow.cpp
           imagerecorder.cpp
           badimagedetector.cpp
           screenshotdisplay.cpp
           resourcestorage.cpp
           plotwidget.cpp
           syncqueue.cpp
           serializerthread.cpp
)

set(QT_HEADERS ${INCLUDE_DIR}/qmainwindow.h
               ${INCLUDE_DIR}/qvideosurface.h
               ${INCLUDE_DIR}/qvideowidget.h
               ${INCLUDE_DIR}/qvideosource.h
               ${INCLUDE_DIR}/qsettingswindow.h
               ${INCLUDE_DIR}/imagerecorder.h
               ${INCLUDE_DIR}/badimagedetector.h
               ${INCLUDE_DIR}/screenshotdisplay.h
               ${INCLUDE_DIR}/resourcestorage.h
               ${INCLUDE_DIR}/plotwidget.h
               ${INCLUDE_DIR}/syncqueue.h
               ${INCLUDE_DIR}/serializerthread.h
)

#set(ST_HEADERS ${INCLUDE_DIR}/kinectmotor.h)

QT5_WRAP_CPP(QT_HEADERS_MOC ${QT_HEADERS}) #run MOC

add_executable(${EXEC} ${SOURCE} ${QT_HEADERS_MOC} ${ST_HEADERS} ${QT_HEADERS})

#Added for plotting
qt5_use_modules(${EXEC} Widgets OpenGL Multimedia)

target_link_libraries(${EXEC} ${KINECT2_LIBS} ${Qt5Widgets_LIBRARIES} ${Qt5Multimedia_LIBRARIES} ${OPENGL_LIBRARY} ${LIBPNG_LIBS})

install(TARGETS ${EXEC} DESTINATION .)
install(FILES ${RESOURCES} DESTINATION .)
