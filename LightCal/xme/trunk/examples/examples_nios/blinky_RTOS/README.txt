Manual for creating a new board. Example for Altera CYCLONE xxx NiosII
with freeRTOS OS.

================
=== Requirement:
================
- cmake > 2.8.5
- MS sdk, if used in windows
- add the directory of cross-compiled make to windows PATH (maybe not necessary)
- in cmake, add entry CMAKE_MAKE_PROGRAM with the absolute path of make

=============
=== steps:
=============
- Define cmake variables, which need to specified in CMakeLists.txt of a
project

1. XME_PLATFORM  freertos
2. XME_ARCHITECTURE  NiosII
3. XME_FAMILY  "FPGA"
4. XME_BOARD "CYCLONE3DEVKIT"
5. XME_TARGET "EP3C120F780C7N"

- create component "xme_target_bsp"
  trunk/bsp/NiosII/CYCLONE3DEVKIT/bsp (The IO address map from XME macros to bsp address or macros)
  trunk/bsp/NiosII/CYCLONE3DEVKIT/hal (for the moment empty)
  trunk/bsp/NiosII/CYCLONE3DEVKIT/gen_bsp (for the bsp project generated from NIOS ide)
  
- create
  trunk/xme/ports/arch/NiosII (xme IP interface implementation using bsp api)
  
- trunk/external/FreeRTOS/CMakeLists.txt (NiosII port speicific)
- trunk/tools/cmake/FindXME.cmake 

- generate source files 
  make build; cd build
  cmake -G "Eclipse CDT4 - Unix Makefiles" ..
  (or use CMake gui)
  
- trunk/tools/toolchain
	create arch specifc directory, e.g., NiosII
	modify ToolchainSourceryCodebench and tell XME where are cross compiler
	

==========
=== Bugs:
==========
XME building systems
- building binary always has C_FLAG and LINK_FLAG, should only contain LINK_FLAG
	-xc in C_FLAG will hang the ld
- multiple copies of a same .a file during linking
- cannot specify the order of .a file during linking
- cannot exclude lwip or use other TCP/IP implementation

	
==========
=== TODO:
==========
- test the lwip functionality
- Need a tutorial for how to build under windows, screenshot!
- FreeRTOS:
	- no below functions for NIOS ports
	void vPortEnterCritical( void ) {}
	void vPortExitCritical( void ) {}

- Use freeRTOS 7.1.0, not the one in the XME trunk. Need to check compliance

- Manual for building bsp from a Nios design, load image, ...
- Put the patch, i.e., objdump, nios-insert to the CMake

- There are two places needed to specify the bsp directory
	FreeRTOS/CMakeList.txt & FindXME.cmake. (need to merge?)
	
- TDMA for arm
=================
=== How-to build:
=================
- the directory for the shell scripts for linking/patching are hard-coded
- set configCHECK_FOR_STACK_OVERFLOW in FreeRTOSConfig.h to 0
- replace the generated FreeRTOSConfig.h with the one in the patch
- Hard-coding of the bsp dir in the link.sh, need to change for each new setup
- after CMake generation, run the copy.sh 
- system.h: comment macro  ALT_ENHANCED_INTERRUPT_API_PRESENT