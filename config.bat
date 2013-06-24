@ECHO off
REM ========================================
REM A DOS batch file to run cmake for PVIZ3 developement
REM 
REM USAGE 
REM
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
SETX ACTIVEMQCPP_HOME %PVIZ3DEV_HOME%\lib\activemq-cpp-library-3.4.0
SETX APR_HOME %PVIZ3DEV_HOME%\lib\httpd-2.2-x64
SETX PROTOBUF_HOME %PVIZ3DEV_HOME%\lib\protobuf-2.4.1

REM SET PATH=%QT_DIR%\bin;%VTK_DIR%\bin;%BOOST_DIR%;%PATH%;

if /I %PVIZ3DEV_GTYPE% == nmake SET CMAKE_GENERATOR="NMake Makefiles"
if /I %PVIZ3DEV_GTYPE% == vs SET CMAKE_GENERATOR="Visual Studio 10 Win64"
REM if %GTYPE% == "3" SET CMAKE_GENERATOR="Eclipse CDT4 - NMake Makefiles"

ECHO ### Generator : %CMAKE_GENERATOR%

REM SET PATH=%PATH%;C:\Program Files (x86)\CMake 2.8\bin;
SETX BOOST_ROOT %PVIZ3DEV_BOOST_DIR%

cmake -G %CMAKE_GENERATOR% ^
-DCMAKE_BUILD_TYPE:STRING=%PVIZ3DEV_TYPE% ^
-DBoost_INCLUDE_DIR:PATH=%PVIZ3DEV_BOOST_DIR%\include ^
-DActivemqcpp_INCLUDE_DIR:PATH=%ACTIVEMQCPP_HOME%\include ^
-DVTK_DIR:PATH=%PVIZ3DEV_VTK_DIR% ^
-DCMAKE_INSTALL_PREFIX:PATH=%cd%/install ^
-DPVIZRPC_CLIENT_ENABLE:BOOL=OFF ^
-DCMAKE_PREFIX_PATH:PATH=%PVIZ3DEV_QT_DIR%;%PVIZ3DEV_VTK_DIR%;%PVIZ3DEV_BOOST_DIR% ^
%PVIZ3_SRC%

REM -DQT_QMAKE_EXECUTABLE:FILEPATH=%QT_DIR%/bin/qmake.exe 
