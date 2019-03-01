cmake_minimum_required(VERSION 3.0.0)

project(WeeveCommon)

# WeeveCommon
set(WOS_ROOT_DIR ${PROJECT_SOURCE_DIR})

# WOS object
include(wos.cmake)

# Build Static Library
add_library(${WOS_TARGET} STATIC ${WOS_TARGET_SRCS} ${WOS_TARGET_INCS})

target_include_directories(${WOS_TARGET} PRIVATE ${WOS_TARGET_INC_DIRS})
target_link_libraries(${WOS_TARGET} ${WOS_TARGET_DEPENDENCY_LIBS})


#Install WCL library and public headers
install(TARGETS ${WOS_TARGET} DESTINATION lib)
install(FILES ${WOS_TARGET_PUBLIC_HEADERS} DESTINATION include)

