@echo off

if "%PVIZ3DEV_HOME%" == "" (
	echo PVIZ3DEV_HOME environment variable is not set.
	echo It needs to point to the root of pviz3 source directory.
	echo E.g., set PVIZ3DEV_HOME=%USERPROFILE%\pviz3
	exit /b 1
)

call %PVIZ3DEV_HOME%\dev\win\build-comm.bat %*
if ERRORLEVEL 1 exit /b 1

REM ========================================
REM Main
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
