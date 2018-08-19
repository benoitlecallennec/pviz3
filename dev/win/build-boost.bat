@echo off

if "%PVIZ3DEV_HOME%" == "" (
	echo PVIZ3DEV_HOME environment variable is not set.
	echo It needs to point to the root of pviz3 source directory.
	echo E.g., set PVIZ3DEV_HOME=%USERPROFILE%\pviz3
	exit /b 1
)

call %PVIZ3DEV_HOME%\dev\win\build-comm.bat %*
if ERRORLEVEL 1 exit /b 1

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
