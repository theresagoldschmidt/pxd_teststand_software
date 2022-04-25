set(
	LIBMODBUS_SRC
	"modbus.c"
	"modbus-data.c"
	"modbus-rtu.c"
	"modbus-tcp.c"
)

set(
	LIBMODBUS_HDR_PUBLIC
	"modbus.h"
	"modbus-rtu.h"
	"modbus-tcp.h"
	"modbus-version.h"
)

set(
	LIBMODBUS_HDR
	${LIBMODBUS_HDR_PUBLIC}
	"modbus-private.h"
	"modbus-rtu-private.h"
	"modbus-tcp-private.h"
)

add_library(
	libmodbus
	STATIC
	${LIBMODBUS_SRC}
	${LIBMODBUS_HDR}
)



install(
	FILES ${LIBMODBUS_HDR_PUBLIC}
	DESTINATION "${CMAKE_SOURCE_DIR}/include"
)

if (WIN32)
	install(
		FILES
		"${CMAKE_CURRENT_BINARY_DIR}/Debug/libmodbus.lib"
		"${CMAKE_CURRENT_BINARY_DIR}/Debug/libmodbus.pdb"
		DESTINATION "${CMAKE_SOURCE_DIR}/lib/win32/debug"
	)
	install(
		FILES
		"${CMAKE_CURRENT_BINARY_DIR}/Release/libmodbus.lib"
		DESTINATION "${CMAKE_SOURCE_DIR}/lib/win32/release"
	)
else (WIN32)
	message (FATAL "Please specify install rules for your platform!")
endif (WIN32)
