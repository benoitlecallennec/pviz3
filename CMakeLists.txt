cmake_minimum_required(VERSION 2.8)

PROJECT(PVIZ3) 

IF (APPLE)
##  SET (SDKROOT "/Developer/SDKs/MacOSX10.7.sdk")
##  SET (CMAKE_OSX_SYSROOT "${SDKROOT}")
set (CMAKE_OSX_ARCHITECTURES x86_64)
ENDIF(APPLE)

add_subdirectory(extern/qtpropertybrowser-2.5_1/src)
##add_subdirectory(extern/nbclient)
add_subdirectory(pvizmodel)
add_subdirectory(src)

