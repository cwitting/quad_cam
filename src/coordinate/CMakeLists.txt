cmake_minimum_required(VERSION 2.8)
project( stars )
find_package( OpenCV )
include_directories( ${OpenCV_INCLUDE_DIRS} )
add_executable( stars stars.cpp )
target_link_libraries( stars ${OpenCV_LIBS} )