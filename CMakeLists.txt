cmake_minimum_required(VERSION 3.2)
project(JLog VERSION 0.2.0)
set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg -g")

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS_DEBUG "-O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

SET(CMAKE_BUILD_TYPE "Release")

# Define GNU standard installation directories
# Provides install directory variables as defined by the GNU Coding Standards.
include(GNUInstallDirs)

include_directories(include/JLog)
include_directories(src)

add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(bench)

option(JLOG_INSTALL "Generate the install target" ON)

if(JLOG_INSTALL)
    message(STATUS "Generating install")
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
endif()
