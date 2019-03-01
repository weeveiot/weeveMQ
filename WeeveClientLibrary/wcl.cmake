cmake_minimum_required(VERSION 3.0.0)

#Set WCL Root-Paths
set(WCL_SRC_DIR ${WCL_ROOT_DIR}/src)
set(WCL_PUBLIC_INCLUDES_DIR ${WCL_ROOT_DIR}/include)
set(WCL_EXTERNAL_DIR ${WCL_ROOT_DIR}/external)

# This is WOS dependency.
set(WOS_COMMON_INCLUDES_DIR ${WOS_ROOT_DIR}/include)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DLOG_LEVEL=${LOG_LEVEL}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLOG_LEVEL=${LOG_LEVEL}")

if(${LIB_SMP_ROLE} STREQUAL "MQTTS_CLIENT")
    add_definitions(-DSMP_MQTTS_CLIENT)
elseif(${LIB_SMP_ROLE} STREQUAL "MQTTS_BROKER")
    add_definitions(-DSMP_MQTTS_BROKER)
else()
    message(FATAL_ERROR "library build role type is not defined.")
endif()

#WCL public headers i.e. interfaces for WCL application developer
set(WCL_PUBLIC_HEADERS ${WCL_PUBLIC_INCLUDES_DIR}/wclCommon.h
                       ${WCL_PUBLIC_INCLUDES_DIR}/wclConfig.h
                       ${WCL_PUBLIC_INCLUDES_DIR}/wclSmp.h
                       ${WCL_PUBLIC_INCLUDES_DIR}/wclTypes.h
                       )

#WCL common component
set(WCL_COMMON_ROOT_DIR ${WCL_SRC_DIR}/com)
set(WCL_COMMON_SRCS ${WCL_COMMON_ROOT_DIR}/wclCommon.c)

#WCL SMP component
set(WCL_SMP_ROOT_DIR ${WCL_SRC_DIR}/smp)
set(WCL_SMP_INCS     ${WCL_SMP_ROOT_DIR}/smp.h
                     ${WCL_SMP_ROOT_DIR}/smpInternal.h
                     ${WCL_SMP_ROOT_DIR}/smpGlobalCreds.h
                     ${WCL_SMP_ROOT_DIR}/smpInternalUtils.h
                     )
set(WCL_SMP_SRCS     ${WCL_SMP_ROOT_DIR}/smp.c
                     ${WCL_SMP_ROOT_DIR}/smpInternal.c
                     ${WCL_SMP_ROOT_DIR}/smpGlobalCreds.c
                     ${WCL_SMP_ROOT_DIR}/smpInternalUtils.c
                     )

# WeeveOSCommon Headers
set(WOS_COMMON_INCS    ${WOS_COMMON_INCLUDES_DIR}/wosCborCert.h
                       ${WOS_COMMON_INCLUDES_DIR}/wosCert.h
                       ${WOS_COMMON_INCLUDES_DIR}/wosCommon.h
                       ${WOS_COMMON_INCLUDES_DIR}/wosCrypto.h
                       ${WOS_COMMON_INCLUDES_DIR}/wosLog.h
                       ${WOS_COMMON_INCLUDES_DIR}/wosMemory.h
                       ${WOS_COMMON_INCLUDES_DIR}/wosMsgCommon.h
                       ${WOS_COMMON_INCLUDES_DIR}/wosMsgSmp.h
                       ${WOS_COMMON_INCLUDES_DIR}/wosStorage.h
                       ${WOS_COMMON_INCLUDES_DIR}/wosString.h
                       ${WOS_COMMON_INCLUDES_DIR}/wosTypes.h
                       )

# WeeveOSCommon Implementation
set(WCL_WOS_SRCS            ${WCL_SRC_DIR}/wos/crypto/wosCert.c
                            ${WCL_SRC_DIR}/wos/misc/wosLogLibC.c
                            ${WCL_SRC_DIR}/wos/misc/wosMemoryLibC.c
                            ${WCL_SRC_DIR}/wos/misc/wosStringLibC.c
                            )

# Libraries
if(${CRYPTO} STREQUAL "TOMCRYPT")
    set(TOMCRYPT_LIB_NAME tomcrypt)
    if(${WCL_LIB_TYPE} STREQUAL "static")
        find_library(LIB_TOMCRYPT_STATIC NAMES lib${TOMCRYPT_LIB_NAME}.a PATHS ${WCL_EXTERNAL_DIR}/libtomcrypt NO_DEFAULT_PATH)
        message(STATUS "tomcrypt: " ${LIB_TOMCRYPT_STATIC})
    elseif(${WCL_LIB_TYPE} STREQUAL "shared")
        find_library(LIB_TOMCRYPT_SHARED NAMES lib${TOMCRYPT_LIB_NAME}.so PATHS ${WCL_EXTERNAL_DIR}/libtomcrypt/.libs NO_DEFAULT_PATH)
        message(STATUS "tomcrypt: " ${LIB_TOMCRYPT_SHARED})
    else()
        message(FATAL_ERROR "library type is not defined.")
    endif()
    set(TOMCRYPT_INC_DIR ${WCL_EXTERNAL_DIR}/libtomcrypt/src/headers)

    set(TOMMATH_LIB_NAME tommath)
    if(${WCL_LIB_TYPE} STREQUAL "static")
        find_library(LIB_TOMMATH_STATIC NAMES lib${TOMMATH_LIB_NAME}.a PATHS ${WCL_EXTERNAL_DIR}/libtommath NO_DEFAULT_PATH)
        message(STATUS "tommath: " ${LIB_TOMMATH_STATIC})
    elseif(${WCL_LIB_TYPE} STREQUAL "shared")
        find_library(LIB_TOMMATH_SHARED NAMES lib${TOMMATH_LIB_NAME}.so PATHS ${WCL_EXTERNAL_DIR}/libtommath/.libs NO_DEFAULT_PATH)
        message(STATUS "tommath: " ${LIB_TOMMATH_SHARED})
    else()
        message(FATAL_ERROR "library type is not defined.")
    endif()
    set(TOMMATH_INC_DIR ${WCL_EXTERNAL_DIR}/libtommath)

    list(APPEND WCL_WOS_SRCS ${WCL_SRC_DIR}/wos/crypto/wosCryptoLibtom.c)
else()
    message(FATAL_ERROR "crypto type is not defined.")
endif()

if(${STORAGE} STREQUAL "STDC_FILE")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DWOS_STORAGE_IMPL_STDC")
    list(APPEND WCL_WOS_SRCS ${WCL_SRC_DIR}/wos/storage/wosStorageFile.c)
	message(STATUS "Storage: STD-C file based")
elseif(${STORAGE} STREQUAL "REDIS")
	find_library(LIB_HIREDIS NAMES libhiredis.a PATHS ${WCL_EXTERNAL_DIR}/hiredis NO_DEFAULT_PATH)
	set(HIREDIS_INC_DIR ${WCL_EXTERNAL_DIR}/hiredis/)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DWOS_STORAGE_IMPL_REDIS")
    list(APPEND WCL_WOS_SRCS ${WCL_SRC_DIR}/wos/storage/wosStorageRedis.c)
	message(STATUS "LIB_HIREDIS: " ${LIB_HIREDIS})
else()
    message(FATAL_ERROR "storage type is not defined.")
endif()

#Set WCL target
if(${LIB_SMP_ROLE} STREQUAL "MQTTS_CLIENT")
    set(WCL_TARGET wcl_client)
elseif(${LIB_SMP_ROLE} STREQUAL "MQTTS_BROKER")
    set(WCL_TARGET wcl_broker)
endif()
set(WCL_TARGET_SRCS ${WCL_COMMON_SRCS} ${WCL_SMP_SRCS} ${WCL_WOS_SRCS})
set(WCL_TARGET_INCS ${WCL_SMP_INCS} ${WOS_COMMON_INCS})
set(WCL_TARGET_INC_DIRS ${WCL_PUBLIC_INCLUDES_DIR} ${WOS_COMMON_INCLUDES_DIR} ${WCL_SMP_ROOT_DIR} ${TOMCRYPT_INC_DIR} ${HIREDIS_INC_DIR})
if(${WCL_LIB_TYPE} STREQUAL "static")
    set(WCL_TARGET_DEPENDENCY_STATIC_LIBS ${LIB_TOMCRYPT_STATIC} ${LIB_TOMMATH_STATIC})
elseif(${WCL_LIB_TYPE} STREQUAL "shared")
    set(WCL_TARGET_DEPENDENCY_SHARED_LIBS ${LIB_TOMCRYPT_SHARED} ${LIB_TOMMATH_SHARED})
endif()
set(WCL_TARGET_PUBLIC_HEADERS ${WCL_PUBLIC_HEADERS}
                              ${WOS_COMMON_INCLUDES_DIR}/wosTypes.h)


