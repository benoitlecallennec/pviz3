
#!/bin/bash

build_comm () {
    if [ -z $PVIZ3DEV_HOME ]; then
        echo PVIZ3DEV_HOME environment variable is not set.
        echo It needs to point to the root of pviz3 source directory.
        echo E.g., export PVIZ3DEV_HOME=$HOME/pviz3
        exit
    fi

    if [ -z $PVIZ3DEV_WORKSPACE ]; then
        echo PVIZ3DEV_WORKSPACE environment variable is not set. 
        echo It needs to point a workspace directory to build pviz3. 
        echo E.g., export PVIZ3DEV_WORKSPACE=$HOME/pviz3_build
        exit
    fi

    if [ -z $PVIZ3DEV_ARCHIVEDIR ]; then
        echo PVIZ3DEV_ARCHIVEDIR environment variable is not set. 
        echo It needs to point a directory contains boost and qt archives
        echo E.g., export PVIZ3DEV_ARCHIVEDIR=$HOME/Downloads
        exit
    fi

    if [ -z $QT_ROOT ]; then
        echo QT_ROOT environment variable is not set. 
        echo It needs to point a directory contains qt installation
        echo E.g., export QT_ROOT=$HOME/Qt/5.11.1/clang_64
        exit
    fi
}

build_vtk () {
    ## ========================================
    ## Build VTK
    ## ========================================

    if [ ! -d $SRCDIR/VTK-$VTK_VER ]; then
        cd $SRCDIR
        tar xvf $PVIZ3DEV_ARCHIVEDIR/VTK-$VTK_VER.tar.gz
    fi

    WORKDIR=$BUILDIR/VTK-$VTK_VER-QT-$QT_VER
    echo === Building VTK $VTK_VER
    echo === WORKDIR: $WORKDIR
    if [ -d $WORKDIR ]; then
        rm -rf $WORKDIR
    fi
    mkdir -p $WORKDIR
    cd $WORKDIR

    cmake \
        -DCMAKE_BUILD_TYPE:STRING=Release \
        -DVTK_Group_Qt=OFF \
        -DModule_vtkGUISupportQt=ON \
        -DModule_vtkGUISupportQtOpenGL=ON \
        -DBUILD_SHARED_LIBS:BOOL=ON \
        -DVTK_QT_VERSION=5 \
        -DCMAKE_INSTALL_PREFIX=$VTK_PREFIX \
        $SRCDIR/VTK-$VTK_VER
    make -j4 install
}

build_boost () {
    ## ========================================
    ## Build Boost
    ## ========================================

    if [ ! -d $SRCDIR/boost_$BOOST_VER ]; then
        cd $SRCDIR
        tar xvf $PVIZ3DEV_ARCHIVEDIR/boost_$BOOST_VER.tar.gz
        cd $SRCDIR/boost_$BOOST_VER
        ./bootstrap.sh
    fi

    WORKDIR=$BUILDIR/boost_$BOOST_VER
    echo === Building Boost $BOOST_VER
    echo === WORKDIR: $WORKDIR
    if [ -d $WORKDIR ]; then
        rm -rf $WORKDIR
    fi
    mkdir -p $WORKDIR


    cd $SRCDIR/boost_$BOOST_VER
    ./b2 \
        toolset=clang \
        linkflags="-stdlib=libc++" \
        threading=multi \
        --without-mpi \
        --build-dir=$WORKDIR \
        --prefix=$BOOST_PREFIX \
        -j4 install
}

build_pviz3 () {
    ## ========================================
    ## Build pviz3
    ## ========================================

    if [ ! -d $SRCDIR/boost_$BOOST_VER ]; then
        cd $SRCDIR
        tar xvf $PVIZ3DEV_ARCHIVEDIR/boost_$BOOST_VER.tar.gz
        cd $SRCDIR/boost_$BOOST_VER
        ./bootstrap.sh
    fi

    WORKDIR=$BUILDIR/pviz3
    echo === Building pviz3
    echo === WORKDIR: $WORKDIR
    if [ -d $WORKDIR ]; then
        rm -rf $WORKDIR
    fi
    mkdir -p $WORKDIR

    cd $WORKDIR

    VTK_HOME=/Users/jyc/sw/vtk/6.3.0-qt5.11.1/clang
    QT_HOME=/Users/jyc/Qt/5.11.1/clang_64
    BOOST_HOME=/Users/jyc/sw/boost/1.59.0/clang

    export PATH=$QT_HOME/bin:$PATH
    CMAKE_PREFIX_PATH=$QT_ROOT:$VTK_PREFIX:$BOOST_PREFIX \
    cmake \
        -DCMAKE_INSTALL_PREFIX=$PVIZ3_PREFIX \
        $PVIZ3DEV_HOME
    make -j4 install
    cpack
}

## ========================================
## Default environmental values
## ========================================
export ARCH=clang
export QT_VER=5.11.1
export VTK_VER=6.3.0
export BOOST_VER=1_59_0

export VTK_PREFIX=$PVIZ3DEV_WORKSPACE/sw/vtk/$VTK_VER-qt$QT_VER/$ARCH
export BOOST_PREFIX=$PVIZ3DEV_WORKSPACE/sw/boost/$BOOST_VER/$ARCH
export PVIZ3_PREFIX=$PVIZ3DEV_WORKSPACE/bin/pviz3

export PATH=$QT_ROOT/bin:$PATH

export BUILDIR=$PVIZ3DEV_WORKSPACE/build
export SRCDIR=$PVIZ3DEV_WORKSPACE/src

if [ ! -d $BUILDIR ]; then
    mkdir -p $BUILDIR
fi

if [ ! -d $SRCDIR ]; then
    mkdir -p $SRCDIR
fi

## ========================================
## Build
## ========================================

build_comm
build_vtk
build_boost
build_pviz3
