cmake_minimum_required(VERSION 3.18)

project(CameraStreamer)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)

file(GLOB shared_sources CameraStreamShared/*.cpp)
add_library(shared ${shared_sources})
target_link_libraries(shared ${OpenCV_LIBS})
target_include_directories(shared PUBLIC CameraStreamShared ${OpenCV_INCLUDE_DIRS})

file(GLOB server_sources CameraStreamServer/*.cpp)
add_executable(server ${server_sources})
target_link_libraries(server shared pthread ${OpenCV_LIBS})

file(GLOB client_sources CameraStream/*.cpp)
add_executable(client ${client_sources})
target_link_libraries(client shared ${OpenCV_LIBS})