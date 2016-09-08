#!/bin/bash
function usage {
    echo "USAGE : `basename $0` path-to-pviz3.app"
}

if [ $# -lt 1 ]  ; then
    usage
    exit
fi

PVIZ3_APP=$1
pushd $PVIZ3_APP
OBJS=`find ./ -name pviz3 -or -name "*.dylib" -type f -or -name "Qt*" -type f`
for OBJ in $OBJS; do
  echo "Reading ... $OBJ"
  LIBS=`otool -L $OBJ | tail +2 | awk '{ print $1 }' | sed 's/@executable_path/./g'`
  for LIB in $LIBS; do
    if [ ${LIB:0:16} != "/System/Library/" ] && [ ${LIB:0:9} != "/usr/lib/" ]; then
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
