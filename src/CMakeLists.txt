cmake_minimum_required(VERSION 2.8.11)

#PROJECT(PVIZ3)
#SET(CMAKE_PREFIX_PATH $ENV{HOME}/usr/vtk)

set(PVIZ3_VERSION_MAJOR "0")
set(PVIZ3_VERSION_MINOR "9")
set(PVIZ3_VERSION_PATCH "0")

FIND_PACKAGE(VTK REQUIRED)
INCLUDE(${VTK_USE_FILE}) # include UseVTK.cmake
##message(VTK_RUNTIME_LIBRARY_DIRS ${VTK_RUNTIME_LIBRARY_DIRS})

set(Boost_USE_STATIC_LIBS   ON)
set(Boost_USE_MULTITHREADED ON)
add_definitions(-DBOOST_ALL_NO_LIB)

IF (APPLE OR UNIX)
  find_package(Boost 1.44 REQUIRED COMPONENTS system signals filesystem iostreams program_options)
ELSE ()
  find_package(Boost 1.44 REQUIRED COMPONENTS system signals filesystem iostreams program_options)
ENDIF ()
include_directories (${Boost_INCLUDE_DIRS})
message(Boost_LIBRARIES ${Boost_LIBRARIES})

find_package( ZLIB REQUIRED )
if ( ZLIB_FOUND )
  include_directories( ${ZLIB_INCLUDE_DIRS} )
endif( ZLIB_FOUND )
message(ZLIB_LIBRARIES ${ZLIB_LIBRARIES})

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
  )

IF (APPLE)
  LIST (APPEND SRCS osxHelper.mm)
ENDIF()

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

IF(WIN32)
  SET(SRCS ${SRCS} CMakeSetup.rc)
ELSE (APPLE)
  SET(SRCS ${SRCS} Icons/pviz.icns)
  SET(MACOSX_BUNDLE_ICON_FILE pviz.icns)
  SET(MACOSX_BUNDLE_BUNDLE_NAME PlotViz)
  SET_SOURCE_FILES_PROPERTIES(Icons/pviz.icns PROPERTIES
    MACOSX_PACKAGE_LOCATION Resources)
ENDIF()

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
  ${ZLIB_LIBRARIES}
  ##${Protobuf_LIBRARIES}
  ${VTK_LIBRARIES}
  Qt5::Core
  Qt5::Widgets
  Qt5::Gui
  Qt5::Concurrent
  Qt5::OpenGL
  )

IF (APPLE)
  set_target_properties(pviz3 PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Info.plist)
ENDIF()

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

## Deployment
IF (APPLE)
  ## Use macdeployqt for OSX
  ADD_CUSTOM_COMMAND(TARGET pviz3 COMMAND echo Run macdeployqt:)
  ADD_CUSTOM_COMMAND(TARGET pviz3 COMMAND which macdeployqt)
  ADD_CUSTOM_COMMAND(TARGET pviz3 COMMAND echo macdeployqt ARGS ${CMAKE_CURRENT_BINARY_DIR}/pviz3.app -libpath=${VTK_INSTALL_PREFIX}/lib)
  ADD_CUSTOM_COMMAND(TARGET pviz3 COMMAND macdeployqt ARGS ${CMAKE_CURRENT_BINARY_DIR}/pviz3.app -libpath=${VTK_INSTALL_PREFIX}/lib)

  INSTALL(DIRECTORY ${PROJECT_SOURCE_DIR}/data DESTINATION .
    PATTERN ".*" EXCLUDE)
ELSE (WIN32)
  FILE(GLOB VTK_REDIST_FILES ${VTK_RUNTIME_LIBRARY_DIRS}/*.dll)
  FILE(GLOB ZLIB_REDIST_FILES ${ZLIB_INCLUDE_DIRS}/../bin/*.dll)
  
  INSTALL(FILES ${VTK_REDIST_FILES} DESTINATION bin)
  INSTALL(FILES ${WIN_REDIST_FILES} DESTINATION bin)
  INSTALL(FILES ${ZLIB_REDIST_FILES} DESTINATION bin)
  
  message(STATUS "Found the following QT libraries")
  SET(QT_DLLS Qt5::Core Qt5::Widgets Qt5::Gui Qt5::Concurrent Qt5::OpenGL)
  foreach(_DLL ${QT_DLLS})
    get_target_property(_LOC ${_DLL} LOCATION)
    LIST(APPEND QT_REDIST_FILES ${_LOC})
    message(STATUS "  ${_LOC}")
  endforeach()
  INSTALL(FILES ${QT_REDIST_FILES} DESTINATION bin)
  
  message(STATUS "Found the following QT plug-ins")
  set(QT_PLUGINS ${Qt5Widgets_PLUGINS} ${Qt5Gui_PLUGINS} ${Qt5Core_PLUGINS} ${Qt5Concurrent_PLUGINS} ${Qt5Svg_PLUGINS})
  foreach(_PLUGIN ${QT_PLUGINS})
    get_target_property(_LOC ${_PLUGIN} LOCATION)
    get_filename_component(_DIR ${_LOC} DIRECTORY)
    message(STATUS "  ${_LOC}")
    list(APPEND QT_PLUGINS_DIR ${_DIR})
  endforeach()

  list(REMOVE_DUPLICATES QT_PLUGINS_DIR)
  install(DIRECTORY ${QT_PLUGINS_DIR} DESTINATION bin
    PATTERN "*.pdb" EXCLUDE)

  INSTALL(DIRECTORY ${PROJECT_SOURCE_DIR}/data DESTINATION .
    PATTERN "*.txt"
    PATTERN "*.pviz")
ENDIF()

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "PlotViz")
SET(CPACK_PACKAGE_VENDOR "SALSA")
IF (WIN32)
  SET(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE.txt")
ELSE (APPLE)
  ## Causing error with DragNDrop (2016/08)
  ##SET(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE.txt")
ENDIF()
SET(CPACK_PACKAGE_VERSION_MAJOR ${PVIZ3_VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${PVIZ3_VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${PVIZ3_VERSION_PATCH})

IF (WIN32)
  # There is a bug in NSI that does not handle full unix paths properly. Make
  # sure there is at least one set of four (4) backlasshes.
  ##message("${PROJECT_SOURCE_DIR}\\\\src\\\\Icons\\\\pviz.ico")
  SET(CPACK_GENERATOR "NSIS")
  SET(CPACK_PACKAGE_INSTALL_DIRECTORY "PVIZ3")
  SET(CPACK_NSIS_MUI_ICON "${PROJECT_SOURCE_DIR}\\\\src\\\\Icons\\\\pviz.ico")
  SET(CPACK_NSIS_MUI_UNIICON  "${PROJECT_SOURCE_DIR}\\\\src\\\\Icons\\\\pviz.ico")
  SET(CPACK_PACKAGE_ICON "${PROJECT_SOURCE_DIR}\\\\etc\\\\pviz_installer_header.bmp")
  SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\pviz3.exe")
  SET(CPACK_NSIS_DISPLAY_NAME "PVIZ3")
  SET(CPACK_NSIS_HELP_LINK "http://salsahpc.indiana.edu/pviz3")
  SET(CPACK_NSIS_URL_INFO_ABOUT "http://salsahpc.indiana.edu/pviz3")
  SET(CPACK_NSIS_CONTACT "jychoi@indiana.edu")
  SET(CPACK_NSIS_MODIFY_PATH ON)

  # File types association:
  SET(CPACK_NSIS_DEFINES "
    !include ${PROJECT_SOURCE_DIR}/CMakeModules\\\\packaging\\\\FileAssociation.nsh
    ")
  
  SET(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
    \\\${registerExtension} \\\"\\\$INSTDIR\\\\bin\\\\pviz3.exe\\\"	\\\".pviz\\\" \\\"PlotViz Data File\\\"
    ")

  SET(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "
    \\\${unregisterExtension} \\\".pviz\\\" \\\"PlotViz Data File\\\"
    ")
ELSE (APPLE)
  SET(CPACK_GENERATOR "DragNDrop")
ENDIF()
SET(CPACK_PACKAGE_EXECUTABLES pviz3 Pviz3)
INCLUDE(CPack)

