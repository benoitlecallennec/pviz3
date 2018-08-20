# PlotViz3 Installation

In general, PlotViz3 depends on the following libraries and tools
* Qt
* VTK
* Boost
* CMake

The following environment variables needs to be set before using building scripts included in `dev\win` or `dev\unix`:
* PVIZ3DEV_HOME: the root of pviz3 source directory
* PVIZ3DEV_WORKSPACE: workspace directory to build pviz3
* PVIZ3DEV_ARCHIVEDIR: directory contains depending archives, such as vtk, boost, zlib, etc

More detailed instructions are as follows.

## Mac OSX

It has been tested with the following libraries:
* QT 5.11.1
* VTK 1.6.30
* Boost 1.59.0

Building and install script is `dev/unix/build.sh`

Here is a sketch of steps

1. Install binary QT libraries downloaded from `http://qt.io`

2. Define the following environment variables (values can be changed)
```
export PVIZ3DEV_HOME=$HOME/project/pviz3
export PVIZ3DEV_WORKSPACE=$HOME/project/pviz3_build
export PVIZ3DEV_ARCHIVEDIR=$HOME/src
export QT_ROOT=$HOME/Qt/5.11.1/clang_64
```

3. Download the following archives in `$PVIZ3DEV_ARCHIVEDIR`
* `https://www.vtk.org/files/release/6.3/VTK-6.3.0.tar.gz`
* `https://sourceforge.net/projects/boost/files/boost/1.59.0/boost_1_59_0.tar.gz/download`

4. Run `build.sh` script
```
build.sh
```

## Windows

It has been tested with the following libraries:
* QT 5.11.1
* VTK 1.6.30
* Boost 1.68.0
* Zlib 1.2.11

1. Install binary QT libraries downloaded from `http://qt.io`

2. Define the following environment variables (values can be changed)
```
set PVIZ3DEV_HOME=%USERPROFILE%\pviz3
set PVIZ3DEV_WORKSPACE=%USERPROFILE%\pviz3_build
set PVIZ3DEV_ARCHIVEDIR=%USERPROFILE%\Downloads
```

3. Download the following archives in `%PVIZ3DEV_ARCHIVEDIR%`
* `https://www.vtk.org/files/release/6.3/VTK-6.3.0.zip`
* `https://dl.bintray.com/boostorg/release/1.68.0/source/boost_1_68_0.zip`
* `https://zlib.net/zlib-1.2.11.tar.gz`

4. Run `build.bat` script
```
build.bat
```
