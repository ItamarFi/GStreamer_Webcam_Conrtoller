cmake_minimum_required(VERSION 3.0.0)
project(webcam_recorder VERSION 0.1.0)

include(CTest)
enable_testing()

find_package(PkgConfig REQUIRED)

pkg_search_module(GST REQUIRED gstreamer-1.0>=1.4
    gstreamer-sdp-1.0>=1.4
    gstreamer-video-1.0>=1.4
    gstreamer-app-1.0>=1.4)

add_executable(webcam_recorder main.cpp VideoStream.cpp SubVideoStream.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

include(CPack)

target_include_directories(webcam_recorder PRIVATE ${GST_INCLUDE_DIRS})
target_link_libraries(webcam_recorder ${GST_LIBRARIES})
