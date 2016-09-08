cmake_minimum_required(VERSION 2.8.11)

#PROJECT(PVIZ3)
#SET(CMAKE_PREFIX_PATH $ENV{HOME}/usr/vtk)

set(PVIZ3_VERSION_MAJOR "0")
set(PVIZ3_VERSION_MINOR "9")
set(PVIZ3_VERSION_PATCH "0")

FIND_PACKAGE(VTK REQUIRED)
INCLUDE(${VTK_USE_FILE}) # include UseVTK.cmake
##message(VTK_RUNTIME_LIBRARY_DIRS ${VTK_RUNTIME_LIBRARY_DIRS})

set(Boost_USE_STATIC_LIBS   OFF)
set(Boost_USE_MULTITHREADED ON)

IF (APPLE OR UNIX)
  find_package(Boost 1.44 REQUIRED COMPONENTS system signals filesystem iostreams program_options)
ELSE ()
  find_package(Boost 1.44 REQUIRED COMPONENTS system signals filesystem iostreams program_options zlib)
ENDIF ()
include_directories (${Boost_INCLUDE_DIRS})

set(CMAKE_INCLUDE_CURRENT_DIR ON)
#set(CMAKE_AUTOMOC ON)
FIND_PACKAGE(Qt5Widgets)
FIND_PACKAGE(Qt5Concurrent)

SET(QTPROPERTYBROWSER_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/extern/qtpropertybrowser-2.5_1/src)
SET(QTPROPERTYBROWSER_LIBRARY_DIRS ${PROJECT_SOURCE_DIR}/extern/qtpropertybrowser-2.5_1/src)
##SET(QTPROPERTYBROWSER_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/extern/QtPropertyBrowser/src)
##SET(QTPROPERTYBROWSER_LIBRARY_DIRS ${PROJECT_SOURCE_DIR}/extern/QtPropertyBrowser/src)
SET(PVIZMODEL_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/pvizmodel/src)

# support for out-of-source build
INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_BINARY_DIR} #this is where ui_SimpleViewUI.h is generated
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${QTPROPERTYBROWSER_INCLUDE_DIRS}
  ${PVIZMODEL_INCLUDE_DIRS}
  ##${NBCLIENT_INCLUDE_DIRS}
  )

LINK_DIRECTORIES(
  ${QTPROPERTYBROWSER_LIBRARY_DIRS}
  )

# Set your files and resources here
SET(SRCS
  main.cpp
  mainwindow.cpp
  pvizwidget.cxx
  pvizsync.h pvizsync.cxx
  pvizinteractorstyle.h pvizinteractorstyle.cxx
  vtkInteractorStyleRubberBand2Pick.h vtkInteractorStyleRubberBand2Pick.cxx
  #vtkCubeAxes2Actor.h vtkCubeAxes2Actor.cxx
  pvizExtractSelectedPolyDataIds.h pvizExtractSelectedPolyDataIds.cxx
  #PvizPoint.cpp PvizPoint.h
  #PvizCluster.cpp PvizCluster.h
  #PvizModel.cpp PvizModel.h
  aboutdialog.cpp
  helpdialog.cpp
  #message.pb.cc message.pb.h
  osxHelper.mm
  )

SET(UIS
  UI/MainWindow.ui
  UI/MainControlsToolbar.ui
  UI/aboutdialog.ui
  UI/helpdialog.ui
  )

SET(HDRS
  mainwindow.h
  pvizwidget.h
  aboutdialog.h
  helpdialog.h
  )

SET(RSCS
  pviz.qrc
  )

QT5_WRAP_UI(UI_SRCS ${UIS})
QT5_WRAP_CPP(MOC_SRCS ${HDRS})
QT5_ADD_RESOURCES(RC_SRCS ${RSCS})

##-IF(Q_WS_WIN)
##-  SET(SRCS ${SRCS} CMakeSetup.rc)
##-ENDIF(Q_WS_WIN)

SET(SRCS ${SRCS} Icons/pviz.icns)
SET(MACOSX_BUNDLE_ICON_FILE pviz.icns)
SET(MACOSX_BUNDLE_BUNDLE_NAME PlotViz)
SET_SOURCE_FILES_PROPERTIES(Icons/pviz.icns PROPERTIES
  MACOSX_PACKAGE_LOCATION Resources)

SOURCE_GROUP("Resources" FILES
  ${UIS}
  )

SOURCE_GROUP("Generated" FILES
  ${UI_SRCS}
  ${MOC_SRCS}
  ${RC_SRCS}
  )

## Remove WIN32 to run in console
ADD_EXECUTABLE(pviz3 WIN32 MACOSX_BUNDLE
  ${SRCS}
  ${HDRS}
  ${UI_SRCS}
  ${MOC_SRCS}
  ${RC_SRCS}
  )

TARGET_LINK_LIBRARIES(pviz3
  ##QVTK
  QtSolutions_PropertyBrowser
  ##QtPropertyBrowser
  pvizmodel
  ##nbclient
  ##${QT_LIBRARIES}
  ${Boost_LIBRARIES}
  ##${Protobuf_LIBRARIES}
  ${VTK_LIBRARIES}
  Qt5::Core
  Qt5::Widgets
  Qt5::Gui
  Qt5::Concurrent
  )

set_target_properties(pviz3 PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Info.plist)

FIND_PACKAGE(Git)
IF(GIT_FOUND)
  EXECUTE_PROCESS (COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_REVISION OUTPUT_STRIP_TRAILING_WHITESPACE)
  SET(PVIZ3_REVISION ${GIT_REVISION})
ELSE()
  SET(PVIZ3_REVISION 0000)
ENDIF()

SET(PVIZ3_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/version.h.in ${CMAKE_CURRENT_BINARY_DIR}/version.h)

INSTALL(TARGETS pviz3
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  BUNDLE DESTINATION .
  )

FILE(GLOB VTK_REDIST_FILES ${VTK_RUNTIME_LIBRARY_DIRS}/*.dylib)
SET(VTK_REDIST_DIR pviz3.app/Contents/Frameworks)
SET(QT_REDIST_DIR pviz3.app/Contents/Frameworks)
SET(QT_REDIST_PLUGINS_DIR pviz3.app/Contents/PlugIns)
SET(QT_REDIST_RESOURCES_DIR pviz3.app/Contents/Resources)
  
INSTALL(FILES ${VTK_REDIST_FILES} DESTINATION ${VTK_REDIST_DIR})
INSTALL(FILES ${Boost_LIBRARIES} DESTINATION ${VTK_REDIST_DIR})
  
## We will use macdeployqt
##-FOREACH(QT_MODULE Qt5::Core Qt5::Widgets Qt5::Gui Qt5::Concurrent)
##-  get_target_property(QT_MODULE_PATH ${QT_MODULE} LOCATION)
##-  get_filename_component(QT_MODULE_ABS_PATH ${QT_MODULE_PATH} REALPATH)
##-  INSTALL(FILES ${QT_MODULE_ABS_PATH} DESTINATION ${QT_REDIST_DIR})
##-ENDFOREACH()

foreach(plugin ${Qt5Gui_PLUGINS})
  get_target_property(_loc ${plugin} LOCATION)
  get_filename_component(_dir ${_loc} DIRECTORY)
  get_filename_component(_dirname ${_dir} NAME)
  INSTALL(FILES ${_loc} DESTINATION ${QT_REDIST_PLUGINS_DIR}/${_dirname})
endforeach()

INSTALL(CODE "EXECUTE_PROCESS(COMMAND which macdeployqt)")
INSTALL(CODE "EXECUTE_PROCESS(COMMAND echo macdeployqt ${CMAKE_INSTALL_PREFIX}/pviz3.app)")
INSTALL(CODE "EXECUTE_PROCESS(COMMAND macdeployqt ${CMAKE_INSTALL_PREFIX}/pviz3.app)")
INSTALL(CODE "EXECUTE_PROCESS(COMMAND chmod -R g+rX,o+rX ${CMAKE_INSTALL_PREFIX}/pviz3.app)")
INSTALL(CODE "EXECUTE_PROCESS(COMMAND find ${CMAKE_INSTALL_PREFIX}/pviz3.app -type l -exec chmod -h g+rX,o+rX {} \;)")

INSTALL(CODE "EXECUTE_PROCESS(COMMAND chmod -R g+rX,o+rX _CPack_Packages)")
INSTALL(CODE "EXECUTE_PROCESS(COMMAND find _CPack_Packages -type l -exec chmod -h g+rX,o+rX {} \;)")

## Run macdeployqt before cpack-ing
## First, find location
execute_process(COMMAND find ${PROJECT_BINARY_DIR}/_CPack_Packages -name "pviz3.app"
  OUTPUT_VARIABLE _cpack_dirs)
separate_arguments (_cpack_dirs UNIX_COMMAND ${_cpack_dirs})

## Then, run macdeployqt
FOREACH(_dir ${_cpack_dirs})
  INSTALL(CODE "EXECUTE_PROCESS(COMMAND echo macdeployqt ${_dir})")
  INSTALL(CODE "EXECUTE_PROCESS(COMMAND macdeployqt ${_dir})")
ENDFOREACH()

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "PlotViz")
SET(CPACK_PACKAGE_VENDOR "SALSA")
## Causing error with DragNDrop (2016/08)
#SET(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE.txt")
SET(CPACK_PACKAGE_VERSION_MAJOR ${PVIZ3_VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${PVIZ3_VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${PVIZ3_VERSION_PATCH})

SET(CPACK_GENERATOR "DragNDrop")
SET(CPACK_PACKAGE_EXECUTABLES pviz3 Pviz3)
INCLUDE(CPack)

