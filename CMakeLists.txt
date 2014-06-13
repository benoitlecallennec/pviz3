cmake_minimum_required(VERSION 2.8)

PROJECT(PVIZ3) 

## Not working
##IF (APPLE)
##  set (CMAKE_OSX_DEPLOYMENT_TARGET 10.9)
##  set (SDKROOT /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.9.sdk)
##  set (CMAKE_OSX_SYSROOT ${SDKROOT})
##  set (CMAKE_OSX_ARCHITECTURES x86_64)
##ENDIF(APPLE)

add_subdirectory(extern/qtpropertybrowser-2.5_1/src)
##add_subdirectory(extern/nbclient)
add_subdirectory(pvizmodel)
add_subdirectory(src)

