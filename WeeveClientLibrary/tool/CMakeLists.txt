cmake_minimum_required(VERSION 3.0.0)

project(WeeveClientLibraryTest)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLOG_LEVEL=${LOG_LEVEL}")

if (CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall –pedantic –std=iso9899:1999 -Wtraditional -Wshadow -Wpointer-arith -Wcast-qual -Wcast-align -Wstrict-prototypes -Wmissing-prototypes -Wconversion")
    message(STATUS "CMAKE_CXX_FLAGS: " ${CMAKE_CXX_FLAGS})
endif()

set(WCL_RELEASE_DIR ${PROJECT_SOURCE_DIR}/../export)

# WeeveClientLibrary
set(WCL_LIB_NAME libwcl_client.a)
set(WCL_INC_DIR ${WCL_RELEASE_DIR}/include)
find_library(LIB_WCL NAMES ${WCL_LIB_NAME} PATHS ${WCL_RELEASE_DIR}/lib)
message(STATUS "LIB_WCL: " ${LIB_WCL})

set (W_CERT_TOOL_SRC ${PROJECT_SOURCE_DIR}/WeeveCertificateTool.c)
# set (W_CERT_TOOL_NAME "WeeveCertTool")

# Include
include_directories(${WCL_INC_DIR})

# Build each file as one executable
add_executable (WeeveCertTool ${W_CERT_TOOL_SRC})
target_link_libraries(WeeveCertTool ${LIB_WCL})
