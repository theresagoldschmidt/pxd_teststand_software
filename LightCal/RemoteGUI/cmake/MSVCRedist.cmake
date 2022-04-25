# MSVC runtime
if (WIN32)
macro(_FIND_MSVC_REDIST VCVERS)
	#message(STATUS "Looking for MSVC Redistributable Executable for MSVC Version ${VCVERS}")
	set(SDKVERS "2.0")
	if(${VCVERS} EQUAL 8)
		set(SDKVERS "2.0")
	endif()
	if(${VCVERS} EQUAL 9)
		set(SDKVERS "3.5")
	endif()
	IF (MSVC${VCVERS}0)
		FIND_PROGRAM(
			MSVC_REDIST NAMES
			vcredist_${CMAKE_MSVC_ARCH}/vcredist_${CMAKE_MSVC_ARCH}.exe
			PATHS
			"[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VCExpress\\${VCVERS}.0;InstallDir]/../../SDK/v${SDKVERS}/BootStrapper/Packages/"
			"[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\${VCVERS}.0;InstallDir]/../../SDK/v${SDKVERS}/BootStrapper/Packages/"
			"[HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\Microsoft\\VisualStudio\\${VCVERS}.0;InstallDir]/../../SDK/v${SDKVERS}/BootStrapper/Packages/"
		)
		GET_FILENAME_COMPONENT(vcredist_name "${MSVC_REDIST}" NAME)
		INSTALL(PROGRAMS ${MSVC_REDIST} COMPONENT SupportFiles DESTINATION bin)
		SET(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "ExecWait '\\\"$INSTDIR\\\\bin\\\\${vcredist_name}\\\" /install /q'")
		set (CPACK_COMPONENT_SUPPORTFILES_REQUIRED 1)
		set (CPACK_COMPONENT_SUPPORTFILES_DISPLAY_NAME "Support files")
		#message(STATUS "MSVC_REDIST: ${MSVC_REDIST}")
	ENDIF(MSVC${VCVERS}0)
endmacro()

set(CMAKE_MSVC_ARCH x86)

if(MSVC80)
_FIND_MSVC_REDIST(8)
endif()

if(MSVC90)
_FIND_MSVC_REDIST(9)
endif()
endif (WIN32)