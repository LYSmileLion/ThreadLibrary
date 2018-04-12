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
        set(CMAKE_C_COMPILER    ${TOOLCHAIN_ROOT}/g++)
        set(CMAKE_CXX_COMPILER  ${TOOLCHAIN_ROOT}/gcc)
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

add_library(${TARGET_OS}_${TARGET_ARCH}_Thread SHARED 
    ${BASE_SRC_LIST}
    ${THREAD_SRC_LIST})

#if(BUILD_SAMPLES)
#	add_executable(file_utils_est ${PROJECT_SOURCE_DIR}/sample/FileTest.cpp)
#	target_link_libraries(file_utils_est ${TARGET_OS}_${TARGET_ARCH}_Thread ${DEPS_LIBS})
#    add_executable(thread_test ${PROJECT_SOURCE_DIR}/sample/ThreadTest.cpp)
#	target_link_libraries(thread_test ${TARGET_OS}_${TARGET_ARCH}_Thread ${DEPS_LIBS})
#	add_executable(mutex_test ${PROJECT_SOURCE_DIR}/sample/MutexTest.cpp)
#	target_link_libraries(mutex_test ${TARGET_OS}_${TARGET_ARCH}_Thread ${DEPS_LIBS})
#	add_executable(logfile_test ${PROJECT_SOURCE_DIR}/sample/LogFileTest.cpp)
#	target_link_libraries(logfile_test ${TARGET_OS}_${TARGET_ARCH}_Thread ${DEPS_LIBS})
#	add_executable(utils_test ${PROJECT_SOURCE_DIR}/sample/UtilsTest.cpp)
#	target_link_libraries(utils_test ${TARGET_OS}_${TARGET_ARCH}_Thread ${DEPS_LIBS})
#	add_executable(LogStream_test ${PROJECT_SOURCE_DIR}/sample/LogStreamTest.cpp)
#	target_link_libraries(LogStream_test ${TARGET_OS}_${TARGET_ARCH}_Thread ${DEPS_LIBS})
#	add_executable(Condition_test ${PROJECT_SOURCE_DIR}/sample/ConditionTest.cpp)
#	target_link_libraries(Condition_test ${TARGET_OS}_${TARGET_ARCH}_Thread ${DEPS_LIBS})
#	add_executable(ThreadPool_test ${PROJECT_SOURCE_DIR}/sample/ThreadPoolTest.cpp)
#	target_link_libraries(ThreadPool_test ${TARGET_OS}_${TARGET_ARCH}_Thread ${DEPS_LIBS})
#	#   add_executable(thread_pool_test ${PROJECT_SOURCE_DIR}/sample/ThreadPoolTest.cpp)
#	#target_link_libraries(thread_pool_test ${TARGET_OS}_${TARGET_ARCH}_Thread ${DEPS_LIBS})
#endif()
#
