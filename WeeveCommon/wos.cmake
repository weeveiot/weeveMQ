cmake_minimum_required(VERSION 3.0.0)

set(WOS_COMMON_EXTERNAL_DIR ${WOS_ROOT_DIR}/external)
set(WOS_COMMON_INCLUDE_DIR ${WOS_ROOT_DIR}/include)
set(WOS_COMMON_SRC_DIR ${WOS_ROOT_DIR}/src)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DLOG_LEVEL=${LOG_LEVEL}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLOG_LEVEL=${LOG_LEVEL}")

if(${LIB_SMP_ROLE} STREQUAL "MQTTS_CLIENT")
    add_definitions(-DSMP_MQTTS_CLIENT)
elseif(${LIB_SMP_ROLE} STREQUAL "MQTTS_BROKER")
    add_definitions(-DSMP_MQTTS_BROKER)
else()
    message(FATAL_ERROR "library build role type is not defined.")
endif()

if(${MESSAGE_PACK} STREQUAL "TINYCBOR")
    set(TINYCBOR_LIB_NAME tinycbor)
    if(${WCL_LIB_TYPE} STREQUAL "static")
        find_library(LIB_TINYCBOR_STATIC NAMES lib${TINYCBOR_LIB_NAME}.a PATHS ${WOS_COMMON_EXTERNAL_DIR}/tinycbor/lib NO_DEFAULT_PATH)
        message(STATUS "tinycbor: " ${LIB_TINYCBOR_STATIC})
    elseif(${WCL_LIB_TYPE} STREQUAL "shared")
        find_library(LIB_TINYCBOR_SHARED NAMES lib${TINYCBOR_LIB_NAME}.so PATHS ${WOS_COMMON_EXTERNAL_DIR}/tinycbor/lib NO_DEFAULT_PATH) #for linux only
        message(STATUS "tinycbor: " ${LIB_TINYCBOR_SHARED})
    else()
        message(FATAL_ERROR "library type is not defined.")
    endif()
    set(TINYCBOR_INCS ${WOS_COMMON_EXTERNAL_DIR}/tinycbor/src)
else()
    message(FATAL_ERROR "message-pack dependency such as 'libtinycbor' is not specified.")
endif()

# WeeveOSCommon headers
set(WOS_COMMON_HEADERS 
                       ${WOS_COMMON_INCLUDE_DIR}/wosCborCert.h
                       ${WOS_COMMON_INCLUDE_DIR}/wosCert.h
                       ${WOS_COMMON_INCLUDE_DIR}/wosCommon.h
                       ${WOS_COMMON_INCLUDE_DIR}/wosCrypto.h
                       ${WOS_COMMON_INCLUDE_DIR}/wosLog.h
                       ${WOS_COMMON_INCLUDE_DIR}/wosMemory.h
                       ${WOS_COMMON_INCLUDE_DIR}/wosMsgCommon.h
                       ${WOS_COMMON_INCLUDE_DIR}/wosMsgSmp.h
                       ${WOS_COMMON_INCLUDE_DIR}/wosStorage.h
                       ${WOS_COMMON_INCLUDE_DIR}/wosString.h
                       ${WOS_COMMON_INCLUDE_DIR}/wosTypes.h
                       )

# Msg component
set(MSG_CBOR_SRCS   ${WOS_COMMON_SRC_DIR}/msg/cbor/msgCborUtils.c
                    ${WOS_COMMON_SRC_DIR}/msg/cbor/wosCborCert.c
                    ${WOS_COMMON_SRC_DIR}/msg/cbor/wosMsgSmp.c
                    )
# SMP message type
set(MSG_INCS      ${WCL_ROOT_DIR}/include)

#Set WOS target name
set(WOS_TARGET wos)
set(WOS_TARGET_SRCS ${MSG_CBOR_SRCS})
set(WOS_TARGET_INCS ${WOS_COMMON_HEADERS})
set(WOS_TARGET_INC_DIRS ${WOS_COMMON_INCLUDE_DIR} ${MSG_INCS} ${TINYCBOR_INCS})
set(WOS_TARGET_DEPENDENCY_STATIC_LIBS ${LIB_TINYCBOR_STATIC})
set(WOS_TARGET_DEPENDENCY_SHARED_LIBS ${LIB_TINYCBOR_SHARED})
set(WOS_TARGET_DEPENDENCY_LIBS ${WOS_TARGET_DEPENDENCY_STATIC_LIBS} ${WOS_TARGET_DEPENDENCY_SHARED_LIBS})
set(WOS_TARGET_PUBLIC_HEADERS ${WOS_COMMON_HEADERS})
