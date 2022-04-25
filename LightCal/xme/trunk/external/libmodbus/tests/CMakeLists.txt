set (NAME "bandwidth_server_one")

add_executable(
	${NAME}
	bandwidth-server-one.c
)

target_link_libraries(
	${NAME}
	libmodbus
)

if (WIN32)
	target_link_libraries(
		${NAME}
		WS2_32.lib
	)
endif (WIN32)



set (NAME "bandwidth_server_many_up")

add_executable(
	${NAME}
	bandwidth-server-many-up.c
)

target_link_libraries(
	${NAME}
	libmodbus
)

if (WIN32)
	target_link_libraries(
		${NAME}
		WS2_32.lib
	)
endif (WIN32)



set (NAME "bandwidth_client")

add_executable(
	${NAME}
	bandwidth-client.c
)

target_link_libraries(
	${NAME}
	libmodbus
)

if (WIN32)
	target_link_libraries(
		${NAME}
		WS2_32.lib
	)
endif (WIN32)



set (NAME "random_test_server")

add_executable(
	${NAME}
	random-test-server.c
)

target_link_libraries(
	${NAME}
	libmodbus
)

if (WIN32)
	target_link_libraries(
		${NAME}
		WS2_32.lib
	)
endif (WIN32)



set (NAME "random_test_client")

add_executable(
	${NAME}
	random-test-client.c
)

target_link_libraries(
	${NAME}
	libmodbus
)

if (WIN32)
	target_link_libraries(
		${NAME}
		WS2_32.lib
	)
endif (WIN32)



set (NAME "unit_test_server")

add_executable(
	${NAME}
	unit-test-server.c
	unit-test.h
)

target_link_libraries(
	${NAME}
	libmodbus
)

if (WIN32)
	target_link_libraries(
		${NAME}
		WS2_32.lib
	)
endif (WIN32)



set (NAME "unit_test_client")

add_executable(
	${NAME}
	unit-test-client.c
	unit-test.h
)

target_link_libraries(
	${NAME}
	libmodbus
)

if (WIN32)
	target_link_libraries(
		${NAME}
		WS2_32.lib
	)
endif (WIN32)



set (NAME "version")

add_executable(
	${NAME}
	version.c
)

target_link_libraries(
	${NAME}
	libmodbus
)

if (WIN32)
	target_link_libraries(
		${NAME}
		WS2_32.lib
	)
endif (WIN32)
