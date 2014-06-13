@ECHO off

REM ========================================
REM Check/Get/Set Env Variables
REM ========================================
IF "%PVIZ3DEV_HOME%" == "" (
	ECHO PVIZ3DEV_HOME environment variable is not defined correctly. 
	EXIT /b 1
)

CALL %PVIZ3DEV_HOME%\bin\build-comm.bat %*
IF ERRORLEVEL 1 GOTO :EOF

REM ========================================
REM Main
REM ========================================

FOR /F "tokens=1,2 delims=." %%i in ("%PVIZ3DEV_BOOST_VER%") do SET PVIZ3DEV_BOOST_VER_SHORT=%%i_%%j
echo %PVIZ3DEV_BOOST_VER_SHORT%
REM move %PVIZ3DEV_BOOST_DIR%\include\boost-%PVIZ3DEV_BOOST_VER_SHORT%
REM GOTO :EOF

IF NOT EXIST %PVIZ3DEV_BOOST_WORKDIR% (
	mkdir %PVIZ3DEV_BOOST_WORKDIR%
)
cd %PVIZ3DEV_BOOST_WORKDIR%

IF NOT EXIST %PVIZ3DEV_BOOST_SRC_DIR% (
	7z x %PVIZ3DEV_WORKSPACE%\Tools\boost_%PVIZ3DEV_BOOST_VER:.=_%.zip
	move boost_%PVIZ3DEV_BOOST_VER:.=_% %PVIZ3DEV_BOOST_SRC_DIR%
)

IF NOT EXIST %PVIZ3DEV_BOOST_BUILD_DIR% (
    mkdir %PVIZ3DEV_BOOST_BUILD_DIR%
)

cd %PVIZ3DEV_BOOST_SRC_DIR%
CALL bootstrap.bat 

REM --build-type=complete ^


bjam ^
--prefix=%PVIZ3DEV_BOOST_DIR% ^
--build-dir=%PVIZ3DEV_BOOST_BUILD_DIR% ^
-sZLIB_SOURCE="C:\pviz3dev_ws\zlib-1.2.7" ^
toolset=msvc ^
address-model=64 ^
install

FOR /F "tokens=1,2 delims=." %%i in ("%PVIZ3DEV_BOOST_VER%") do SET PVIZ3DEV_BOOST_VER_SHORT=%%i_%%j
cd %PVIZ3DEV_BOOST_DIR%\include
move boost-%PVIZ3DEV_BOOST_VER_SHORT%\boost .
rmdir boost-%PVIZ3DEV_BOOST_VER_SHORT%

