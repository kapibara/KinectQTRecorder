
set(EXEC recorder)

set(SOURCE main.cpp qmainwindow.cpp qvideowidget.cpp qvideosurface.cpp kinectmotor.cpp qvideosource.cpp qsettingswindow.cpp framerecorder.cpp)

set(QT_HEADERS ../include/qmainwindow.h ../include/qvideosurface.h ../include/qvideowidget.h ../include/qvideosource.h ../include/framerecorder.h ../include/qsettingswindow.h)
set(ST_HEADERS ../include/kinectmotor.h )

QT5_WRAP_CPP(QT_HEADERS_MOC ${QT_HEADERS}) #run MOC

add_executable(${EXEC} ${SOURCE} ${QT_HEADERS_MOC} ${ST_HEADERS} ${QT_HEADERS})

#target_link_libraries(${EXEC} QtMultimediaKit QtGui OpenNI ${OpenCV_LIBS})
target_link_libraries(${EXEC} ${Qt5Widgets_LIBRARIES} ${Qt5Multimedia_LIBRARIES} OpenNI ${OpenCV_LIBS})

install(TARGETS ${EXEC} DESTINATION .)

