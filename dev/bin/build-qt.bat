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

IF NOT EXIST %PVIZ3DEV_QT_WORKDIR% (
	mkdir %PVIZ3DEV_QT_WORKDIR%
)
cd %PVIZ3DEV_QT_WORKDIR%

IF NOT EXIST %PVIZ3DEV_QT_SRC_DIR% (
    7z x %PVIZ3DEV_WORKSPACE%\Tools\qt-everywhere-opensource-src-%PVIZ3DEV_QT_VER%.zip
	move qt-everywhere-opensource-src-%PVIZ3DEV_QT_VER% %PVIZ3DEV_QT_SRC_DIR%
)

IF NOT EXIST %PVIZ3DEV_QT_BUILD_DIR% (
    mkdir %PVIZ3DEV_QT_BUILD_DIR%
)
cd %PVIZ3DEV_QT_BUILD_DIR%

%PVIZ3DEV_QT_SRC_DIR%\configure.exe -prefix %PVIZ3DEV_QT_DIR% -debug-and-release -platform win32-msvc2008

nmake
nmake install
