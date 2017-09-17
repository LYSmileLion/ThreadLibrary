cmake_minimum_required (VERSION 2.8)
project (ThreadLibrary)

#SET(CMAKE_VERBOSE_MAKEFILE ON)

option (BUILD_SAMPLES "whether to compile samples" ON)
option (BUILD_UNITTESTS "whether to compile unittest" OFF)
option (USE_CPP11 "whether to build using CPP11 flag" ON)
option (USE_GDB   "whether to build using gdb debug flag" OFF)
option (DEBUG     "whether to link debug version of static libraries or sample" OFF)
option (TIME_PRINT     "whether to print time" OFF)


if(NOT TARGET_OS)
    set(TARGET_OS ${CMAKE_SYSTEM_NAME})
endif()

if(NOT TARGET_ARCH)
    set(TARGET_ARCH ${CMAKE_SYSTEM_PROCESSOR})
endif()

if(NOT TOOLCHAIN_ROOT)
    set(TOOLCHAIN_ROOT /usr/bin)
endif()


if(TIME_PRINT)
    add_definitions(-DPRINT_TIME)
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
if (USE_GDB OR DEBUG)
    add_definitions(-DDEBUG)
    message(STATUS "Debug version")
else ()
    add_definitions(-DNDEBUG)
    message(STATUS "Release version")
endif()

if (${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
    set(CXX_COMMON_FLAGS "-O3 -Wall")
    if (USE_GDB)
        set(CXX_COMMON_FLAGS " -O0 -Wall -g -ggdb")
        message(STATUS "you can use GDB debug .")
    endif()
    if (USE_CPP11)
        add_definitions(-DUSE_CPP11)
        set(CPP11_ENABLED_FLAGS "-std=c++11")
    else()
        set(CPP11_ENABLED_FLAGS "-std=c++0x")
    endif()
endif()

message(STATUS "CMAKE_SYSTEM_PROCESSOR: " ${CMAKE_SYSTEM_PROCESSOR})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CPP11_ENABLED_FLAGS} ${CXX_COMMON_FLAGS}  -lm")


set(TOP_API_INCLUDE ${PROJECT_SOURCE_DIR}/include)

include_directories(${TOP_API_INCLUDE})

set(THREAD_SRC ${PROJECT_SOURCE_DIR}/src/Thread.cpp)
set(THREAD_SRC ${THREAD_SRC} ${PROJECT_SOURCE_DIR}/src/FileUtils.cpp)
set(THREAD_SRC ${THREAD_SRC} ${PROJECT_SOURCE_DIR}/src/MutexLock.cpp)
set(THREAD_SRC ${THREAD_SRC} ${PROJECT_SOURCE_DIR}/src/LogFile.cpp)
#set(THREAD_SRC ${THREAD_SRC} ${PROJECT_SOURCE_DIR}/src/ThreadPool.cpp)

if(TARGET_OS STREQUAL "Linux")
    set(DEPS_LIBS
        pthread
        stdc++)
endif()
message(status DEPS_LIBS: "${DEPS_LIBS}")

add_library(${TARGET_OS}_${TARGET_ARCH}_Thread SHARED ${THREAD_SRC})

message(STATUS "lib_name: ${TARGET_OS}_${TARGET_ARCH}_Thread")

if(BUILD_SAMPLES)
	add_executable(file_utils_est ${PROJECT_SOURCE_DIR}/sample/FileTest.cpp)
	target_link_libraries(file_utils_est ${TARGET_OS}_${TARGET_ARCH}_Thread ${DEPS_LIBS})
    add_executable(thread_test ${PROJECT_SOURCE_DIR}/sample/ThreadTest.cpp)
	target_link_libraries(thread_test ${TARGET_OS}_${TARGET_ARCH}_Thread ${DEPS_LIBS})
	add_executable(mutex_test ${PROJECT_SOURCE_DIR}/sample/MutexTest.cpp)
	target_link_libraries(mutex_test ${TARGET_OS}_${TARGET_ARCH}_Thread ${DEPS_LIBS})
	add_executable(logfile_test ${PROJECT_SOURCE_DIR}/sample/LogFileTest.cpp)
	target_link_libraries(logfile_test ${TARGET_OS}_${TARGET_ARCH}_Thread ${DEPS_LIBS})
	#   add_executable(thread_pool_test ${PROJECT_SOURCE_DIR}/sample/ThreadPoolTest.cpp)
	#target_link_libraries(thread_pool_test ${TARGET_OS}_${TARGET_ARCH}_Thread ${DEPS_LIBS})
endif()

