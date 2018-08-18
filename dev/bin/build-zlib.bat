@echo off

if "%PVIZ3DEV_HOME%" == "" (
	echo PVIZ3DEV_HOME environment variable is not set.
	echo It needs to point to the root of pviz3 source directory.
	echo E.g., set PVIZ3DEV_HOME=%USERPROFILE%\pviz3
	exit /b 1
)

call %PVIZ3DEV_HOME%\dev\bin\build-comm.bat %*
if ERRORLEVEL 1 exit /b 1

rem ========================================
rem Main
rem ========================================

if NOT EXIST %SRCDIR%\zlib-%ZLIB_VER% (
    for /f "tokens=1,2,3 delims=." %%i in ("%ZLIB_VER%") do set VER=%%i%%j%%k
    7za x %PVIZ3DEV_ARCHIVEDIR%\zlib%VER%.zip -o%SRCDIR%
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