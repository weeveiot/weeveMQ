cmake_minimum_required(VERSION 3.0.0)

project(WeeveOSCommonTest)

set(WOS_TEST_ROOT_DIR ${PROJECT_SOURCE_DIR})

# WOS object
set(WOS_ROOT_DIR ${PROJECT_SOURCE_DIR}/..)
include(${WOS_TEST_ROOT_DIR}/../wos.cmake)

# Test Sources
set(WOS_TEST_SRCS        unit/TestMsgSmpCbor.cpp
                         unit/TestCborCert.cpp
                         )

# Include log/memory etc. dependency from Rich-OS implementation.
# Log and memory interface is used in WeeveOSCommon components, we need Rich-OS 
# implementation of these interfaces since we are testing WeeveOSCommon components 
# in Rich-OS env.
set(WOS_TEST_DEPENDENCY_SRCS    ${WCL_ROOT_DIR}/src/wos/misc/wosLogLibC.c
                            ${WCL_ROOT_DIR}/src/wos/misc/wosStringLibC.c
                            ${WCL_ROOT_DIR}/src/wos/misc/wosMemoryLibC.c
                            )

#Add GTEST
include (gtest.cmake)

# Build Test Executable and Link to GTest
add_executable(testWosCommon    ${WOS_TEST_SRCS} 
                                ${WOS_TEST_DEPENDENCY_SRCS} 
                                ${WOS_TARGET_SRCS} 
                                ${WOS_TARGET_INCS}
                                )

target_include_directories(testWosCommon PRIVATE ${WOS_TARGET_INC_DIRS})
target_link_libraries(testWosCommon ${WOS_TARGET_DEPENDENCY_LIBS} gtest_main)

#add_test(NAME example_test COMMAND example)


