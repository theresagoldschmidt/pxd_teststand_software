
cmake_minimum_required(VERSION 2.8.6)


set (
	CMAKE_MODULE_PATH
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake"
)

set(NAME PsRemoteControl)

project(${NAME})


   add_library(VISA STATIC "C:/Program Files (x86)/IVI Foundation/VISA/WinNT/lib/msc/visa32.lib")

  add_library(FTDI STATIC "C:/LightCal/FTDI/i386/ftd2xx.lib")

  
  set(VISA_LIB, "C:/Program Files (x86)/IVI Foundation/VISA/WinNT/lib/msc/visa32.lib")

  set(FTDI_LIB, "C:/LightCal/FTDI/i386/ftd2xx.lib")

SET_TARGET_PROPERTIES(FTDI PROPERTIES LINKER_LANGUAGE C)

SET_TARGET_PROPERTIES(VISA PROPERTIES LINKER_LANGUAGE C)

 # target_link_libraries(A B)
 # target_link_libraries(B A)



# include ftdi stuff
set(FTDI_DIR, "C:/LightCal/FTDI" )
set(VISA_DIR," C:/Program Files/IVI Foundation/VISA/Win64/include"    )

set(COMMON_DIR, "C:/LightCal/common" )

INCLUDE_DIRECTORIES(${FTDI_DIR})
INCLUDE_DIRECTORIES(${VISA_DIR})
INCLUDE_DIRECTORIES(${COMMON_DIR})



### Sources, headers and resources ###
set(
	SRCS
	SMU.cpp
	CalManager.cpp
	PsReceiveParse.cpp
	OvpTest.cpp
	PsRemoteControl.cpp
	ftdi.cpp
	StartStop.cpp
	main.cpp
	
)

set(
	HDRS
	CalManager.h
	ftdi.h
	SMU.h
	OvpTest.h
	PsRemoteControl.h
 	StartStop.h
	PsReceiveParse.h
)

set(
	UIS
	server_ui.ui
)


### Qt configuration ###
# Qt modules (QtCore and QtGui are enabled by default)

set (QT_USE_QTMAIN TRUE)
set (QT_USE_QTNETWORK TRUE)
set (QT_USE_QTTEST TRUE)

set (CMAKE_AUTOMOC TRUE)

find_package(Qt4 REQUIRED)

# Add some useful macros and variables
# (QT_USE_FILE is a variable defined by find_package(Qt4) that contains a path to CMake script)
include(${QT_USE_FILE})

# Run rcc on all files from RCS
#qt4_add_resources (RC_SRCS ${RCS} )

# Run uic on .ui files
qt4_wrap_ui (UI_HDRS ${UIS})

# TODO use XME macro here
include_directories (${CMAKE_BINARY_DIR})


set(
	CMAKE_LIBRARY_OUTPUT_DIRECTORY
	${PROJECT_BINARY_DIR}/target
	CACHE PATH
	"Target directory for all libraries"
)
set(
	CMAKE_RUNTIME_OUTPUT_DIRECTORY
	${PROJECT_BINARY_DIR}/target
	CACHE PATH
	"Target directory for all executables"
)
set(
	CMAKE_ARCHIVE_OUTPUT_DIRECTORY
	${PROJECT_BINARY_DIR}/target
	CACHE PATH
	"Target directory for all static libraries"
)



add_executable (
	${NAME}
	WIN32
	${SRCS}
	${HDRS}
	#${RC_SRCS}
	${UI_HDRS}
)

target_link_libraries(
	${NAME}
	 ${VISA_LIB})

target_link_libraries(
	${NAME}
	 ${FTDI})

# TODO use XME macro here
target_link_libraries(
	${NAME}
	${QT_LIBRARIES}
)


### Create installer ###
include (MSVCRedist)
# TODO: cleanup install directory before executing installation
set (CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/install)

install (
	TARGETS
	${NAME}
	RUNTIME DESTINATION bin
)


### Create installer ###
# http://nsis.sourceforge.net/
if (WIN32)
set (CPACK_GENERATOR NSIS)
else (WIN32)
	message (FATAL_ERROR "Need to select cpack generator.")
endif (WIN32)
include(CPack)


