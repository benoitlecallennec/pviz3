@echo off

if "%PVIZ3DEV_HOME%" == "" (
	echo PVIZ3DEV_HOME environment variable is not set.
	echo It needs to point to the root of pviz3 source directory.
	echo E.g., set PVIZ3DEV_HOME=%USERPROFILE%\pviz3
	goto :ERROR
)

if "%PVIZ3DEV_WORKSPACE%" == "" (
	echo PVIZ3DEV_WORKSPACE environment variable is not set. 
	echo It needs to point a workspace directory to build pviz3. 
	echo E.g., set PVIZ3DEV_WORKSPACE=%USERPROFILE%\pviz3_build
	goto :ERROR
)

if "%PVIZ3DEV_ARCHIVEDIR%" == "" (
	echo PVIZ3DEV_ARCHIVEDIR environment variable is not set. 
	echo It needs to point a directory contains boost, qt, and zlib archives
	echo E.g., set PVIZ3DEV_ARCHIVEDIR=%USERPROFILE%\Downloads
	goto :ERROR
)

rem ========================================
rem Default environmental values
rem ========================================
set ARCH=msvc2017_64
set QT_VER=5.11.1
set VTK_VER=6.3.0
set BOOST_VER=1_68_0
set ZLIB_VER=1.2.8

if "%~1" == "" goto MAIN
:GETOPTS
if /I "%~1" == "-h" goto HELP
if /I "%~1" == "-a" set ARCH=%~2& SHIFT
if /I "%~1" == "-qt" set QT_VER=%~2& SHIFT
if /I "%~1" == "-vtk" set VTK_VER=%~2& SHIFT
if /I "%~1" == "-boost" set BOOST_VER=%~2& SHIFT
if /I "%~1" == "-zlib" set ZLIB_VER=%~2& SHIFT
SHIFT
if not "%~1" == "" goto GETOPTS

:MAIN
set QT_ROOT=C:\Qt\%QT_VER%\%ARCH%
set VTK_PREFIX=%PVIZ3DEV_WORKSPACE%\sw\VTK\%VTK_VER%-QT%QT_VER%\%ARCH%
set ZLIB_PREFIX=%PVIZ3DEV_WORKSPACE%\sw\zlib\%ZLIB_VER%\%ARCH%
set BOOST_PREFIX=%PVIZ3DEV_WORKSPACE%\sw\Boost\%BOOST_VER%\%ARCH%
set PVIZ3_PREFIX=%PVIZ3DEV_WORKSPACE%\bin\pviz3

set BUILDIR=%PVIZ3DEV_WORKSPACE%\build
set SRCDIR=%PVIZ3DEV_WORKSPACE%\src
if not exist %PVIZ3DEV_WORKSPACE% mkdir %PVIZ3DEV_WORKSPACE%
if not exist %BUILDIR% mkdir %BUILDIR%
if not exist %SRCDIR% mkdir %SRCDIR%

set PATH=%QT_ROOT%\bin;%PVIZ3DEV_HOME%\dev\bin;%PATH%
echo ========================================
echo PVIZ3DEV_HOME=%PVIZ3DEV_HOME%
echo PVIZ3DEV_WORKSPACE=%PVIZ3DEV_WORKSPACE%
echo PVIZ3DEV_ARCHIVEDIR=%PVIZ3DEV_ARCHIVEDIR%
echo.
echo QT_ROOT=%QT_ROOT%
echo VTK_PREFIX=%VTK_PREFIX%
echo ZLIB_PREFIX=%ZLIB_PREFIX%
echo BOOST_PREFIX=%BOOST_PREFIX%
echo PVIZ3_PREFIX=%PVIZ3_PREFIX%
echo ========================================

if not exist %QT_ROOT% (
	echo [ERROR] QT_ROOT is not set correctly.
	exit /b 1
)

if not exist %PVIZ3DEV_HOME% (
	echo [ERROR] PVIZ3DEV_HOME is not set correctly.
	exit /b 1
)

if not exist %PVIZ3DEV_ARCHIVEDIR% (
	echo [ERROR] PVIZ3DEV_ARCHIVEDIR is not set correctly.
	exit /b 1
)
exit /b 0

:HELP
echo USAGE : COMMAND [OPTIONS]
echo OPTIONS
echo     -a     : architecture (x64 or x86)
echo     -qt    : Qt version
echo     -vtk   : VTK version
echo     -boost : BOOST version
echo     -zlib  : ZLIB version
echo.
exit /b 1

:ERROR
exit /b 1
