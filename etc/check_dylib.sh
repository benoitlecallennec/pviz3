#!/bin/bash
function usage {
    echo "USAGE : `basename $0` path-to-pviz3.app"
}

if [ $# -lt 1 ]  ; then
    usage
    exit
fi

PVIZ3_APP=$1
cd $PVIZ3_APP/Contents/MacOS
LIBS=`otool -L pviz3 | sed '1d' | awk '{ print $1 }' | sed 's/@executable_path/./g'`
for lib in $LIBS; do
    #echo $lib
    if ! [ -a $lib ]; then
        echo $lib ... no
    fi
done
cd $OLDPWD
echo "Done."
