cmake_minimum_required(VERSION 3.2)
project(JLog VERSION 0.2.0)
set(CMAKE_CXX_STANDARD 17)

include_directories(include/JLog)
include_directories(src)

add_subdirectory(src)
add_subdirectory(tests)

message(STATUS "Generating install")

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/installed" CACHE PATH "default install path" FORCE )
endif()

install(
    TARGETS JLog
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(
    DIRECTORY "${PROJECT_SOURCE_DIR}/include/JLog"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    FILES_MATCHING PATTERN "*.h*")
