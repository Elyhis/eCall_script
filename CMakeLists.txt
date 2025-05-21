cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)

project(eCall_skydell_transcription)

set(CMAKE_DEBUG_POSTFIX _d)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG) 
endif()

add_subdirectory(src)
add_subdirectory(inc)
