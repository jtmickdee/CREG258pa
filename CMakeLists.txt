cmake_minimum_required(VERSION 2.8)
project( objDetect )
set(raspicam_DIR "/usr/local/lib/cmake")
find_package( OpenCV REQUIRED )
find_package(raspicam REQUIRED)
add_executable( tennisCam tennisCam.cpp )
target_link_libraries( tennisCam ${OpenCV_LIBS} )
target_link_libraries(tennisCam ${raspicam_CV_LIBS})

target_link_libraries(tennisCam ${raspicam_LIBS} )

