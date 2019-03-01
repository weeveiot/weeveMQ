cmake_minimum_required(VERSION 3.0.0)

project(WeeveClientLibrary)

# Version number
set (WCL_VERSION_MAJOR 1)
set (WCL_VERSION_MINOR 0)
set (WCL_VERSION_REVISION 0)
set (INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

configure_file (
    "${PROJECT_SOURCE_DIR}/CMake_Config.h.in"
    "${INCLUDE_DIR}/wclVersion.h")

include_directories("${INCLUDE_DIR}")

# WeeveOSCommon
set(WOS_ROOT_DIR ${WOS_ROOT_DIR})
include(${WOS_ROOT_DIR}/wos.cmake)

#Set WCL Root-Path
set(WCL_ROOT_DIR ${PROJECT_SOURCE_DIR})
include(${WCL_ROOT_DIR}/wcl.cmake)

# Build Static Library
if(${WCL_LIB_TYPE} STREQUAL "static")
    set(WCL_TARGET_STATIC_LIB ${WCL_TARGET}_static)
    add_library(${WCL_TARGET_STATIC_LIB} STATIC ${WCL_TARGET_SRCS} ${WCL_TARGET_INCS} ${WOS_TARGET_SRCS} ${WOS_TARGET_INCS})
    target_include_directories(${WCL_TARGET_STATIC_LIB} PRIVATE ${WCL_TARGET_INC_DIRS} ${WOS_TARGET_INC_DIRS})

# Build Shared Library
elseif(${WCL_LIB_TYPE} STREQUAL "shared")
    set(WCL_TARGET_SHARED_LIB ${WCL_TARGET})
    add_library(${WCL_TARGET_SHARED_LIB} SHARED ${WCL_TARGET_SRCS} ${WCL_TARGET_INCS} ${WOS_TARGET_SRCS} ${WOS_TARGET_INCS})
    target_include_directories(${WCL_TARGET_SHARED_LIB} PRIVATE ${WCL_TARGET_INC_DIRS} ${WOS_TARGET_INC_DIRS})
    target_link_libraries(${WCL_TARGET_SHARED_LIB} ${WOS_TARGET_DEPENDENCY_SHARED_LIBS} ${WCL_TARGET_DEPENDENCY_SHARED_LIBS})
    set(WCL_SHARED_LIB ${CMAKE_BINARY_DIR}/lib${WCL_TARGET_SHARED_LIB}.so)
else()
    message(FATAL_ERROR "library type is not defined.")
endif()

if(${WCL_LIB_TYPE} STREQUAL "static")
# Archive all external dependency libs(static) with wcl_static TARGET to generate combined static lib
set(OPREFIX object_)
set(WCL_COMB_STATIC_LIB ${CMAKE_BINARY_DIR}/lib${WCL_TARGET}.a)
add_custom_target(wcl_custom ALL
            COMMAND mkdir -p ${OPREFIX}${WCL_TARGET_STATIC_LIB} && cd ${OPREFIX}${WCL_TARGET_STATIC_LIB} &&  ar -x ${CMAKE_BINARY_DIR}/lib${WCL_TARGET_STATIC_LIB}.a
            COMMAND mkdir -p ${OPREFIX}tommath && cd ${OPREFIX}tommath &&  ar -x ${LIB_TOMMATH_STATIC}
            COMMAND mkdir -p ${OPREFIX}tomcrypt && cd ${OPREFIX}tomcrypt &&  ar -x ${LIB_TOMCRYPT_STATIC}
            COMMAND mkdir -p ${OPREFIX}tinycbor && cd ${OPREFIX}tinycbor &&  ar -x ${LIB_TINYCBOR_STATIC}
            COMMAND ar -rcs ${WCL_COMB_STATIC_LIB} ${OPREFIX}*/*.o
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            DEPENDS ${WCL_TARGET_STATIC_LIB}
            )
endif()

#Install WCL library and public headers
set(CMAKE_INSTALL_PREFIX ${WCL_ROOT_DIR}/export)
#install(TARGETS ${WCL_TARGET} DESTINATION lib)
install(FILES ${WCL_COMB_STATIC_LIB} ${WCL_SHARED_LIB} DESTINATION lib)
install(FILES ${WCL_TARGET_PUBLIC_HEADERS} ${WOS_TARGET_PUBLIC_HEADERS} DESTINATION include)
