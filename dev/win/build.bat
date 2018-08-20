@echo off
SETLOCAL EnableDelayedExpansion

call :BUILD_COMM
if %ERRORLEVEL% NEQ 0 exit /b %ERRORLEVEL%

call :BUILD_VTK
if %ERRORLEVEL% NEQ 0 exit /b %ERRORLEVEL%

call :BUILD_ZLIB
if %ERRORLEVEL% NEQ 0 exit /b %ERRORLEVEL%

call :BUILD_BOOST
if %ERRORLEVEL% NEQ 0 exit /b %ERRORLEVEL%

call :BUILD_PVIZ3
if %ERRORLEVEL% NEQ 0 exit /b %ERRORLEVEL%

exit /b 0

:BUILD_COMM
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

set PATH=%QT_ROOT%\bin;%PVIZ3DEV_HOME%\dev\win;%PATH%
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

:BUILD_VTK
REM ========================================
REM VTK
REM ========================================

if not exist %SRCDIR%\VTK-%VTK_VER% (
    7za x %PVIZ3DEV_ARCHIVEDIR%\VTK-%VTK_VER%.zip -o%SRCDIR%
)

cd %BUILDIR%
set WORKDIR=%BUILDIR%\VTK-%VTK_VER%-QT-%QT_VER%
echo.
echo --- Building VTK 
echo --- WORKDIR: %WORKDIR%
echo.

if exist %WORKDIR% rmdir /S /Q %WORKDIR%
mkdir %WORKDIR% & cd %WORKDIR%

cmake -G "NMake Makefiles" ^
  -DCMAKE_BUILD_TYPE=Release ^
  -DVTK_QT_VERSION=5 ^
  -DQT_QMAKE_EXECUTABLE:PATH="%QT_ROOT%\bin\qmake.exe" ^
  -DVTK_Group_Qt=OFF ^
  -DModule_vtkGUISupportQt=ON ^
  -DModule_vtkGUISupportQtOpenGL=ON ^
  -DCMAKE_PREFIX_PATH:PATH="%QT_ROOT%\lib\cmake" ^
  -DBUILD_SHARED_LIBS=ON ^
  -DCMAKE_INSTALL_PREFIX=%VTK_PREFIX% ^
  %SRCDIR%\VTK-%VTK_VER%

where jom
if %ERRORLEVEL% EQU 0 (
  jom install
) else (
  nmake install
)
exit /b %ERRORLEVEL%

:BUILD_ZLIB


rem ========================================
rem ZLIB
rem ========================================

if NOT EXIST %SRCDIR%\zlib-%ZLIB_VER% (
   for /f "tokens=1,2,3 delims=." %%i in ("%ZLIB_VER%") do (
        set ZLIB_VER_SHORT=%%i%%j%%k
    )
    echo -- ZLIB_VER_SHORT: !ZLIB_VER_SHORT!
    7za x %PVIZ3DEV_ARCHIVEDIR%\zlib!ZLIB_VER_SHORT!.zip -o%SRCDIR%
)

cd %BUILDIR%
set WORKDIR=%BUILDIR%\zlib-%ZLIB_VER%
echo.
echo --- Building ZLIB
echo --- WORKDIR: %WORKDIR%
echo.

if exist %WORKDIR% rmdir /S /Q %WORKDIR%
mkdir %WORKDIR% & cd %WORKDIR%

cmake -G "NMake Makefiles" ^
  -DCMAKE_BUILD_TYPE=Release ^
  -DCMAKE_INSTALL_PREFIX=%ZLIB_PREFIX% ^
  %SRCDIR%\zlib-%ZLIB_VER%

nmake install
exit /b %ERRORLEVEL%

:BUILD_BOOST
REM ========================================
REM BOOST
REM ========================================
if not exist %SRCDIR%\boost_%BOOST_VER% (
	7za x %PVIZ3DEV_ARCHIVEDIR%\boost_%BOOST_VER%.zip -o%SRCDIR%
)

cd %BUILDIR%
set WORKDIR=%BUILDIR%\boost_%BOOST_VER%
echo.
echo --- Building BOOST
echo --- WORKDIR: %WORKDIR%
echo.

if exist %WORKDIR% rmdir /S /Q %WORKDIR%
mkdir %WORKDIR% & cd %WORKDIR%

cd %SRCDIR%\boost_%BOOST_VER%
if not exist b2.exe (
	call bootstrap.bat
)

.\b2 ^
	address-model=64 ^
	threading=multi ^
	link=static ^
	--toolset=msvc ^
	--build-dir=%WORKDIR% ^
	--prefix=%BOOST_PREFIX% ^
	-s ZLIB_INCLUDE="%ZLIB_PREFIX%\include" ^
	-s ZLIB_LIBPATH="%ZLIB_PREFIX%\lib" ^
	-s ZLIB_BINARY=zlib ^
	-j4 install
exit /b %ERRORLEVEL%

:BUILD_PVIZ3
REM ========================================
REM PVIZ3
REM ========================================

cd %BUILDIR%
set WORKDIR=%BUILDIR%\pviz3
echo.
echo --- Building PVIZ3
echo --- WORKDIR: %WORKDIR%
echo.

if exist %WORKDIR% rmdir /S /Q %WORKDIR%
mkdir %WORKDIR% & cd %WORKDIR%

for /f "tokens=1,2,3 delims=_" %%i in ("%BOOST_VER%") do set BOOST_VER_SHORT=%%i_%%j
cmake ^
  -G "NMake Makefiles" ^
  -DCMAKE_PREFIX_PATH=%QT_ROOT%;%VTK_PREFIX%;%BOOST_PREFIX%;%ZLIB_PREFIX% ^
  -DCMAKE_BUILD_TYPE:STRING=Release ^
  -DCMAKE_INSTALL_PREFIX:PATH=%PVIZ3_PREFIX% ^
  -DBOOST_INCLUDEDIR=%BOOST_PREFIX%\include\boost-!BOOST_VER_SHORT! ^
  -DBOOST_LIBRARYDIR=%BOOST_PREFIX%\lib ^
  -DBoost_USE_STATIC_LIBS=ON ^
  %PVIZ3DEV_HOME%

nmake install
cpack
exit /b %ERRORLEVEL%

:ERROR
exit /b 1
