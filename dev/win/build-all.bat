call build-vtk
if ERRORLEVEL 1 exit /b 1

call build-zlib
if ERRORLEVEL 1 exit /b 1

call build-boost
if ERRORLEVEL 1 exit /b 1

call build-pviz3
if ERRORLEVEL 1 exit /b 1