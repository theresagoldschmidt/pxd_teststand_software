@echo off

rem Cleanup
rem
taskkill /im arm-none-eabi-gdb.exe /F

rem Execute GDB
C:/BelleII/devenv/toolchain/arm-gcc/bin/arm-none-eabi-gdb.exe %*

