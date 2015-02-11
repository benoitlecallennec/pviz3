#!/bin/bash

install_names ()
{
  OBJ=$1
  for LIB in `otool -L $OBJ | tail +2 | awk '{ print $1}'`; do
    if [ ${LIB:0:15} == "/opt/local/lib/" -o ${LIB:0:7} == "/Users/" ]; then
        BASE=`basename $LIB`
        echo install_name_tool -change $LIB $BASE $OBJ
        install_name_tool -change $LIB $BASE $OBJ
        LIB=$BASE
    fi
    if [ ${LIB:0:1} != "/" ] && [ ${LIB:0:1} != "@" ]; then
      DEST=Libraries
      if [[ ${LIB} == *framework* ]]; then
        DEST=Frameworks
      fi
      echo install_name_tool -change $LIB @executable_path/../$DEST/$LIB $OBJ
      install_name_tool -change $LIB @executable_path/../$DEST/$LIB $OBJ
    fi
  done
}

install_id ()
{
  OBJ=$1
  for LIB in `otool -D $OBJ | tail -n 1 | awk '{ print $1}'`; do
    if [ ${LIB:0:1} == "/" ]; then
        LIB=`basename $LIB`
        echo "Reset id ... " install_name_tool -id $LIB $OBJ
        install_name_tool -id $LIB $OBJ
    fi

    if [ ${LIB:0:1} != "/" ] && [ ${LIB:0:1} != "@" ]; then
      DEST=Libraries
      if [[ ${LIB} == *framework* ]]; then
        DEST=Frameworks
      fi
      echo install_name_tool -id @executable_path/../$DEST/$LIB $OBJ
      install_name_tool -id @executable_path/../$DEST/$LIB $OBJ
    fi
  done
}

FILES=$1
if [ x"$FILES" == "x" ]; then
  FILES=`find install _CPack_Packages -name "pviz3" -or -name "lib*.dylib" -type f -or -name "Qt*" -type f -or -name "phonon" -type f | grep -v -e "Headers" -e "prl"`
fi
for f in $FILES; do
  install_id $f
  install_names $f
done
