cmake_minimum_required(VERSION 3.0.0)
project(JLog VERSION 0.2.0)
set(CMAKE_CXX_STANDARD 17)

include_directories(include)
include_directories(src)

add_subdirectory(src)
add_subdirectory(tests)
