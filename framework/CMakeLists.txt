cmake_minimum_required (VERSION 2.8)
project (ThreadLibrary)

#SET(CMAKE_VERBOSE_MAKEFILE ON)

option (DEBUG     "whether to link debug version of static libraries or sample" OFF)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

if(NOT TARGET_OS)
    set(TARGET_OS ${CMAKE_SYSTEM_NAME})
endif()

if(NOT TARGET_ARCH)
    set(TARGET_ARCH ${CMAKE_SYSTEM_PROCESSOR})
endif()

if(NOT TOOLCHAIN_ROOT)
    set(TOOLCHAIN_ROOT /usr/bin)
endif()

if(TARGET_OS STREQUAL "Linux")
    if(TARGET_ARCH STREQUAL "x86_64")
        set(CMAKE_SYSTEM_RPOCESSOR x86_64)
        set(CMAKE_C_COMPILER    ${TOOLCHAIN_ROOT}/gcc)
        set(CMAKE_CXX_COMPILER  ${TOOLCHAIN_ROOT}/g++)
    else()
        message(FATAL_ERROR "The specified target:${TARGET_OS}-${TARGET_ARCH} is not supported")
    endif()
else()
    message(FATAL_ERROR "The specified target:${TARGET_OS}-${TARGET_ARCH} is not supported")
endif()

## definitions
if (DEBUG)
    add_definitions(-DDEBUG)
    message(STATUS "Debug version")
else ()
    add_definitions(-DNDEBUG)
    message(STATUS "Release version")
endif()

if (${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
    set(CMAKE_CXX_FLAGS "-O3 -Wall -std=c++11")
    if (DEBUG)
        set(CMAKE_CXX_FLAGS " -O0 -Wall -g -ggdb -std=c++11")
        message(STATUS "you can use GDB debug .")
    endif()
endif()

message(STATUS "CMAKE_CXX_FLAGS: " ${CMAKE_CXX_FLAGS})

include(GetModulesInformation)
get_modules_informatrion(base)
get_modules_informatrion(log)
get_modules_informatrion(thread)
get_modules_informatrion(net)

add_library(${TARGET_OS}_${TARGET_ARCH}_Framework SHARED 
    ${BASE_SRC_LIST}
    ${THREAD_SRC_LIST}
    ${LOG_SRC_LIST}
    ${NET_SRC_LIST})
