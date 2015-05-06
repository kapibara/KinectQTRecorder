
set(EXEC recorder)
set(RESOURCES Bad.png
              Ok.png
              boing.wav
)

set(INCLUDE_DIR ${HOME}/include)

set(SOURCE main.cpp
           qmainwindow.cpp
           qvideowidget.cpp
           qvideosurface.cpp
           kinectmotor.cpp
           qvideosource.cpp
           qsettingswindow.cpp
           imagerecorder.cpp
           badimagedetector.cpp
           screenshotdisplay.cpp
           resourcestorage.cpp
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
)

set(ST_HEADERS ${INCLUDE_DIR}/kinectmotor.h)

QT5_WRAP_CPP(QT_HEADERS_MOC ${QT_HEADERS}) #run MOC

add_executable(${EXEC} ${SOURCE} ${QT_HEADERS_MOC} ${ST_HEADERS} ${QT_HEADERS})

#target_link_libraries(${EXEC} QtMultimediaKit QtGui OpenNI ${OpenCV_LIBS})
target_link_libraries(${EXEC} ${Qt5Widgets_LIBRARIES} ${Qt5Multimedia_LIBRARIES} OpenNI ${OpenCV_LIBS} ${IntelSDK_LIBS} ${IntelCW_LIB})

install(TARGETS ${EXEC} DESTINATION .)
install(FILES ${RESOURCES} DESTINATION .)

