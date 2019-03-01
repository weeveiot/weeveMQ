cmake_minimum_required(VERSION 3.0.0)

project(WeeveClientLibraryTest)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLOG_LEVEL=${LOG_LEVEL}")

set(WCL_RELEASE_DIR ${PROJECT_SOURCE_DIR}/../export)

# WeeveClientLibrary
#Currently only testing smp client flavour
set(WCL_LIB_NAME wcl_client)
set(WCL_INC_DIR ${WCL_RELEASE_DIR}/include)
# Specify which type(static or shared) of WCL should be used to test
if(${WCL_LIB_TYPE} STREQUAL "static")
    find_library(LIB_WCL NAMES lib${WCL_LIB_NAME}.a PATHS ${WCL_RELEASE_DIR}/lib)
elseif(${WCL_LIB_TYPE} STREQUAL "shared")
    find_library(LIB_WCL NAMES lib${WCL_LIB_NAME}.so PATHS ${WCL_RELEASE_DIR}/lib)
else()
    message(FATAL_ERROR "wcl library type is not defined.")
endif()
message(STATUS "wcl lib: " ${LIB_WCL})

# Testing Sources
set(WCL_UNIT_TEST_SRCS      unit/TestWosCert.cpp
                            unit/TestWosCrypto.cpp
                            unit/TestWosMemory.cpp
                            unit/TestWosStorage.cpp
                            unit/TestWosString.cpp
                            )

set(WCL_MODULE_TEST_SRCS      module/TestWclSmp.cpp)
set(WCL_TEST_SRCS ${WCL_UNIT_TEST_SRCS} ${WCL_MODULE_TEST_SRCS})

# Library googletest
include (gtest.cmake)

# Include
include_directories(${WCL_INC_DIR})

# Build each file as one executable
foreach(_test_file ${WCL_TEST_SRCS})
    get_filename_component(_test_name ${_test_file} NAME_WE)
    add_executable(${_test_name} ${_test_file})
    # Link
    target_link_libraries(${_test_name} ${LIB_WCL} gtest_main)
endforeach()
