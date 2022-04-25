@echo off

rem Cleanup
rem
taskkill /im openocd.exe /F
taskkill /im arm-none-eabi-gdb.exe /F

rem Execute OpenOCD in a clean path environment in order to avoid sporadic crashes

set PATH=""

"C:/BelleII/devenv/toolchain/openocd/bin/openocd.exe" %*

