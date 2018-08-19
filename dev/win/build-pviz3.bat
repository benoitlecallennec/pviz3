@echo off

if "%PVIZ3DEV_HOME%" == "" (
	echo PVIZ3DEV_HOME environment variable is not set.
	echo It needs to point to the root of pviz3 source directory.
	echo E.g., set PVIZ3DEV_HOME=%USERPROFILE%\pviz3
	exit /b 1
)

call %PVIZ3DEV_HOME%\dev\win\build-comm.bat %*
if ERRORLEVEL 1 exit /b 1

cd %BUILDIR%
set WORKDIR=%BUILDIR%\pviz3
echo.
echo --- Building PVIZ3
echo --- WORKDIR: %WORKDIR%
echo.

if exist %WORKDIR% rmdir /S /Q %WORKDIR%
mkdir %WORKDIR% & cd %WORKDIR%

for /f "tokens=1,2,3 delims=_" %%i in ("%BOOST_VER%") do set VER=%%i_%%j
cmake ^
  -G "NMake Makefiles" ^
  -DCMAKE_PREFIX_PATH=%QT_ROOT%;%VTK_PREFIX%;%BOOST_PREFIX%;%ZLIB_PREFIX% ^
  -DCMAKE_BUILD_TYPE:STRING=Release ^
  -DCMAKE_INSTALL_PREFIX:PATH=%PVIZ3_PREFIX% ^
  -DBOOST_INCLUDEDIR=%BOOST_PREFIX%\include\boost-%VER% ^
  -DBOOST_LIBRARYDIR=%BOOST_PREFIX%\lib ^
  -DBoost_USE_STATIC_LIBS=ON ^
  %PVIZ3DEV_HOME%

nmake install
cpack