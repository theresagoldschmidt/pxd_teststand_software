# We assume that the find script for the Sourcery Codebench toolchain
# is in the same directory as this toolchain specification
#
# We also assume that a make program is in our path
set (_CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH})
set (CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../../cmake)
# find_package (SourceryCodebench REQUIRED)
set (CMAKE_MODULE_PATH ${_CMAKE_MODULE_PATH})

# C and C++ compiler
 set (CMAKE_SYSTEM_NAME "Generic")
# set (CMAKE_C_COMPILER ${SOURCERY_CODEBENCH_C_COMPILER})
# set (CMAKE_CXX_COMPILER ${SOURCERY_CODEBENCH_CXX_COMPILER})
# set (CMAKE_DEBUGGER ${SOURCERY_CODEBENCH_DEBUGGER})
#set (CMAKE_MAKE_PROGRAM "C:/altera/11.0/nios2eds/bin/gnu/H-i686-mingw32/bin/make.exe")
# set (CMAKE_MAKE_PROGRAM "make.exe")
set (CMAKE_C_COMPILER "C:/altera/11.0/nios2eds/bin/gnu/H-i686-mingw32/bin/nios2-elf-gcc.exe")
# set (CMAKE_C_COMPILER "nios2-elf-gcc.exe")
set (CMAKE_CXX_COMPILER "C:/altera/11.0/nios2eds/bin/gnu/H-i686-mingw32/bin/nios2-elf-g++.exe")
set (CMAKE_DEBUGGER "C:/altera/11.0/nios2eds/bin/gnu/H-i686-mingw32/bin/nios2-elf-gdb.exe")
