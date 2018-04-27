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

include(GetModulesSamples)
get_modules_samples(base)
get_modules_samples(log)
get_modules_samples(net)
get_modules_samples(thread)
get_modules_samples(log)
list(APPEND sample_list 
    ${LOG_SRC}
    ${NET_SRC})

if(TARGET_OS STREQUAL "Linux")
    if(TARGET_ARCH STREQUAL "x86_64")
        set(framework_lib_name  Linux_x86_64_Framework)
        set(framework_lib_path  ${PROJECT_SOURCE_DIR}/../framework/build)
        link_directories(${framework_lib_path})
        message(STATUS "lib path : ${framework_lib_path} lib name : ${framework_lib_name}")
    else()
        message(FATAL_ERROR "The specified target:${TARGET_OS}-${TARGET_ARCH} is not supported")
    endif()
else()
    message(FATAL_ERROR "The specified target:${TARGET_OS}-${TARGET_ARCH} is not supported")
endif()

if (NOT EXISTS ${framework_lib_path}/lib${framework_lib_name}.so)
    message(FATAL_ERROR "Please compile framework firstly before compile samples")
endif()


foreach(sample ${sample_list})
    string(REGEX MATCH "[^/]+$" file_name ${sample})
    string(REPLACE ".cpp" "" base_name ${file_name})
    message(STATUS "sample name : ${base_name} ${sample}")
    add_executable(${base_name} ${sample})
    target_link_libraries(${base_name}
        ${framework_lib_name}
        pthread
        stdc++)
endforeach(sample)
#add_executable(file_utils_est ${PROJECT_SOURCE_DIR}/sample/FileTest.cpp)
#target_link_libraries(file_utils_est)
