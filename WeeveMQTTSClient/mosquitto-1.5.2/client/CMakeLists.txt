include_directories(${mosquitto_SOURCE_DIR} ${mosquitto_SOURCE_DIR}/lib
			${STDBOOL_H_PATH} ${STDINT_H_PATH})
link_directories(${mosquitto_BINARY_DIR}/lib)

set(shared_src client_shared.c client_shared.h)

if (${WITH_SRV} STREQUAL ON)
	add_definitions("-DWITH_SRV")
endif (${WITH_SRV} STREQUAL ON)

if (${WITH_WEEVE_SMP} STREQUAL ON)
    find_library(LIB_WCL_CLIENT_SHARED NAMES lib${WCL_CLIENT_LIB_NAME}.so PATHS ${WCL_LIB_DIR})
endif()

add_executable(mosquitto_pub pub_client.c ${shared_src})
add_executable(mosquitto_sub sub_client.c sub_client_output.c ${shared_src})

target_link_libraries(mosquitto_pub libmosquitto ${LIB_WCL_CLIENT_SHARED})
target_link_libraries(mosquitto_sub libmosquitto ${LIB_WCL_CLIENT_SHARED})

install(TARGETS mosquitto_pub RUNTIME DESTINATION "${BINDIR}" LIBRARY DESTINATION "${LIBDIR}")
install(TARGETS mosquitto_sub RUNTIME DESTINATION "${BINDIR}" LIBRARY DESTINATION "${LIBDIR}")
