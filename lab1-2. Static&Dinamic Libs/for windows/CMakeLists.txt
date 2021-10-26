cmake_minimum_required(VERSION 3.17)
project(lab)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_library(libNumber STATIC Number/Number.cpp Number/Number.h)

add_library(libVector SHARED Vector/Vector.cpp Vector/Vector.h)

add_executable(lab main.cpp)

target_link_libraries(libVector PRIVATE libNumber)
target_link_libraries(lab PUBLIC libNumber)
target_link_libraries(lab PUBLIC libVector)

