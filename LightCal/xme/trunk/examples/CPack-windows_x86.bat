REM
REM Copyright (c) 2011-2012, fortiss GmbH.
REM Licensed under the Apache License, Version 2.0.
REM
REM Use, modification and distribution are subject to the terms specified
REM in the accompanying license file LICENSE.txt located at the root directory
REM of this software distribution. A copy is available at
REM http://chromosome.fortiss.org/.
REM
REM This file is part of CHROMOSOME.
REM
REM $Id$
REM
REM Author:
REM         Michael Geisinger <geisinger@fortiss.org>
REM

@ECHO off

:: If CMake is not in the PATH, specify the installation directory below:
SET CMAKEPATH=C:\Program Files (x86)\CMake 2.8

:: Path to MSBuild (adapt to your .NET Framework version)
SET MSBUILDPATH=%WINDIR%\Microsoft.NET\Framework\v4.0.30319

:: Define the file and project name(s) of the examples to compile/install
SET PROJECTLIST=tutorial;coordinator;templateGenerator

:: Path to the source directory
SET SOURCEDIR=.

:: Path to the build directory
SET BUILDDIR=.\build

SETLOCAL ENABLEDELAYEDEXPANSION



:: Make source path absolute
PUSHD .
CD %SOURCEDIR%
SET REALSOURCEDIR=%CD%
POPD
ECHO Source directory: %REALSOURCEDIR%

:: Make build path absolute
PUSHD .
IF NOT EXIST "%BUILDDIR%\nul" MKDIR "%BUILDDIR%"
CD %BUILDDIR%
SET REALBUILDDIR=%CD%
POPD
ECHO Build directory: %REALBUILDDIR%



:: Try to find CMake
CALL cmake.exe --version
IF ERRORLEVEL 9009 GOTO CMAKENOTINPATH
GOTO CMAKEINPATH

:CMAKENOTINPATH
CALL "%CMAKEPATH%\bin\cmake.exe --version"
IF ERRORLEVEL 9009 GOTO CMAKENOTFOUND
GOTO CMAKEFOUND

:CMAKENOTFOUND
ECHO Unable to find CMake. Either put it in your PATH or specify the location manually in this batch file.
GOTO END

:CMAKEINPATH
SET REALCMAKE=cmake.exe
SET REALCPACK=cpack.exe
ECHO CMake found in PATH
GOTO BUILDENVOK

:CMAKEFOUND
SET REALCMAKE=%CMAKEPATH%\bin\cmake.exe
SET REALCPACK=%CMAKEPATH%\bin\cpack.exe
ECHO CMake found at: %CMAKEPATH%\bin
GOTO BUILDENVOK



:: Prepare build system and build
:BUILDENVOK

SET START_TIMESTAMP=%DATE% %TIME%
ECHO ----------------------------------------
ECHO  Build started on %START_TIMESTAMP%
ECHO ----------------------------------------



:PROJECTLOOP
	IF "%PROJECTLIST%" EQU "" GOTO BUILDDONE

	FOR /f "delims=;" %%a in ("!PROJECTLIST!") do set PROJECT=%%a

	ECHO ----------------------------------------
	ECHO  Cleaning example "%PROJECT%"...
	ECHO ----------------------------------------

	REM Clean up project build directory
	CALL DEL /S /Q "%REALBUILDDIR%\%PROJECT%" > nul

	ECHO ----------------------------------------
	ECHO  Configuring example "%PROJECT%"...
	ECHO ----------------------------------------

	REM Configure build system for project
	IF NOT EXIST "%REALBUILDDIR%\%PROJECT%\nul" MKDIR "%REALBUILDDIR%\%PROJECT%"
	CD "%REALBUILDDIR%\%PROJECT%"
	CALL "%REALCMAKE%" -G "Visual Studio 10" "%REALSOURCEDIR%\%PROJECT%"
	IF ERRORLEVEL 1 GOTO CMAKEERROR

	ECHO ----------------------------------------
	ECHO  Building example "%PROJECT%"...
	ECHO ----------------------------------------

	REM Build solution
	CALL "%MSBUILDPATH%\MSBuild.exe" "%REALBUILDDIR%\%PROJECT%\%PROJECT%.sln" /m /p:Configuration=Release
	IF ERRORLEVEL 1 GOTO BUILDERROR

	REM Build documentation (not built by default)
	IF EXIST "%REALBUILDDIR%\%PROJECT%\%PROJECT%_doc.vcxproj" (
		CALL "%MSBUILDPATH%\MSBuild.exe" "%REALBUILDDIR%\%PROJECT%\%PROJECT%_doc.vcxproj" /p:Configuration=Release
		IF ERRORLEVEL 1 GOTO BUILDERROR
	)

	REM Install after building documentation (not built by default)
	CALL "%MSBUILDPATH%\MSBuild.exe" "%REALBUILDDIR%\%PROJECT%\INSTALL.vcxproj" /p:Configuration=Release
	IF ERRORLEVEL 1 GOTO BUILDERROR

:STRIPLOOP
    SET STRIPCHAR=%PROJECTLIST:~0,1%
    SET PROJECTLIST=%PROJECTLIST:~1%
	IF "%PROJECTLIST%" EQU "" GOTO PROJECTLOOP
	IF "%STRIPCHAR%" NEQ ";" GOTO STRIPLOOP

	GOTO PROJECTLOOP
)



:BUILDDONE

:: Run CPack
ECHO ----------------------------------------
ECHO  Creating package...
ECHO ----------------------------------------
CD %REALSOURCEDIR%
CALL "%REALCPACK%" -C Release --config "Tutorial_CPackConfig.cmake"
IF ERRORLEVEL 1 GOTO PACKAGEERROR

ECHO ----------------------------------------
ECHO  All done!        
ECHO ----------------------------------------
GOTO END



:CMAKEERROR
ECHO ========================================
ECHO  Error: A CMake error occurred!
ECHO ========================================
GOTO END

:BUILDERROR
ECHO ========================================
ECHO  Error: A build error occurred!
ECHO ========================================
GOTO END

:PACKAGEERROR
ECHO ========================================
ECHO  Error: A packaging error occurred!
ECHO ========================================
GOTO END



:END

SET END_TIMESTAMP=%DATE% %TIME%

REM Calculate time difference
REM Date difference is not supported, because the date is
REM formatted differently depending on the language settings!

SET START_DATE=%START_TIMESTAMP:~0,10%
::SET START_YEAR=%START_TIMESTAMP:~10,4%
::SET START_MON=%START_TIMESTAMP:~4,2%
::SET START_DATE=%START_TIMESTAMP:~7,2%
SET START_HOUR=%START_TIMESTAMP:~11,2%
SET START_MIN=%START_TIMESTAMP:~14,2%
SET START_SEC=%START_TIMESTAMP:~17,2%
SET START_HSEC=%START_TIMESTAMP:~20,2%

SET END_DATE=%START_TIMESTAMP:~0,10%
::SET END_YEAR=%END_TIMESTAMP:~10,4%
::SET END_MON=%END_TIMESTAMP:~4,2%
::SET END_DATE=%END_TIMESTAMP:~7,2%
SET END_HOUR=%END_TIMESTAMP:~11,2%
SET END_MIN=%END_TIMESTAMP:~14,2%
SET END_SEC=%END_TIMESTAMP:~17,2%
SET END_HSEC=%END_TIMESTAMP:~20,2%

REM Skip total time calculation if date differs (see comment above)
IF NOT "%START_DATE%"=="%END_DATE%" GOTO different_date

SET /A START_HSECS="START_HSEC + (START_SEC * 100) + (START_MIN * 6000) + (START_HOUR * 360000)"
SET /A END_HSECS="END_HSEC + (END_SEC * 100) + (END_MIN * 6000) + (END_HOUR * 360000)"

SET /A DIFF="END_HSECS - START_HSECS"

SET /A DIFF_HOUR="DIFF / 360000"
SET /A DIFF_MIN="(DIFF / 6000) %% 60"
SET /A DIFF_SEC="(DIFF / 100) %% 60"
SET /A DIFF_HSEC="DIFF %% 100"

REM Add leading zeroes if necessary
IF "%DIFF_HOUR%" LSS "10" SET DIFF_HOUR=0%DIFF_HOUR%
IF "%DIFF_MIN%" LSS "10" SET DIFF_MIN=0%DIFF_MIN%
IF "%DIFF_SEC%" LSS "10" SET DIFF_SEC=0%DIFF_SEC%
IF "%DIFF_HSEC%" LSS "10" SET DIFF_HSEC=0%DIFF_HSEC%

ECHO ----------------------------------------
ECHO  Build stopped on %END_TIMESTAMP%
ECHO  Total build time: %DIFF_HOUR%:%DIFF_MIN%:%DIFF_SEC%,%DIFF_HSEC%
ECHO ----------------------------------------



ENDLOCAL
PAUSE
