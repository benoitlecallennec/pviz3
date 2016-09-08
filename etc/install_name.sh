#!/bin/bash

install_id ()
{
  OBJ=$1
  for LIB in `otool -D $OBJ | tail -n 1 | awk '{print $1}'`; do
    if [ ${LIB:0:16} != "/System/Library/" ] && [ ${LIB:0:9} != "/usr/lib/" ] && [ ${LIB:0:1} != "@" ]; then
      LIB=`basename $LIB`
      echo 1:"Reset id ... " install_name_tool -id $LIB $OBJ
      install_name_tool -id $LIB $OBJ

      DEST=Libraries
      echo 2:install_name_tool -id @executable_path/../$DEST/$LIB $OBJ
      install_name_tool -id @executable_path/../$DEST/$LIB $OBJ
    fi
  done
}

install_names ()
{
  OBJ=$1
  for LIB in `otool -L $OBJ | tail +2 | awk '{print $1}'`; do
    if [ ${LIB:0:16} != "/System/Library/" ] && [ ${LIB:0:9} != "/usr/lib/" ] && [ ${LIB:0:1} != "@" ]; then
      BASE=`basename $LIB`
      echo 3:install_name_tool -change $LIB $BASE $OBJ
      install_name_tool -change $LIB $BASE $OBJ
      LIB=$BASE

      DEST=Libraries
      echo 4:install_name_tool -change $LIB @executable_path/../$DEST/$LIB $OBJ
      install_name_tool -change $LIB @executable_path/../$DEST/$LIB $OBJ
    fi
  done
}

copy_libs ()
{
  PVIZ3_APP=$1
  pushd $PVIZ3_APP
  OBJS=`find ./ -name pviz3 -or -name "*.dylib" -type f -or -name "Qt*" -type f`
  for OBJ in $OBJS; do
    echo "Reading ... $OBJ"
    LIBS=`otool -L $OBJ | tail +2 | awk '{ print $1 }' | sed 's/@executable_path/./g'`
    for LIB in $LIBS; do
      if [ ${LIB:0:16} != "/System/Library/" ] && [ ${LIB:0:9} != "/usr/lib/" ] && [ ${LIB:0:1} != "@" ]; then
        LIBNAME=`basename $LIB`
        ##echo "  "$LIBNAME
        if ! [ -a ./Contents/Libraries/$LIBNAME ]; then
          echo "  "$LIB ... no
          cp $LIB ./Contents/Libraries/
        fi
      else
        ##echo "  "$LIBNAME ... skip
        continue
      fi
    done
  done
  popd
  echo "Done."
}

PREFIX=$1
mkdir -p $PREFIX/Contents/Libraries
cp -r $HOME/Qt/5.2.1/clang_64/plugins $PREFIX/Contents
copy_libs $PREFIX

FILES=`find $PREFIX -name "pviz3" -or -name "lib*.dylib" -or -name "Qt*"`
for f in $FILES; do
  install_id $f
  install_names $f
done
