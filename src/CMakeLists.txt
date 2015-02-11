cmake_minimum_required(VERSION 2.8)

#PROJECT(PVIZ3)
#SET(CMAKE_PREFIX_PATH $ENV{HOME}/usr/vtk)

set(PVIZ3_VERSION_MAJOR "0")
set(PVIZ3_VERSION_MINOR "8")
set(PVIZ3_VERSION_PATCH "21") 

FIND_PACKAGE(VTK REQUIRED)
INCLUDE(${VTK_USE_FILE}) # include UseVTK.cmake

## Try not to use OpenMP. It's error-prone
## But, it
##FIND_PACKAGE(OpenMP)
##if(OPENMP_FOUND)
  ##message(OpenMP_C_FLAGS ${OpenMP_C_FLAGS})
  ##message(OpenMP_CXX_FLAGS ${OpenMP_CXX_FLAGS})
  ##message(OpenMP_EXE_LINKER_FLAG ${OpenMP_EXE_LINKER_FLAG})
  ##set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  ##set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
  ##set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
##endif()

## Need to remove start-up console appearance
IF (WIN32)
  SET (QT_USE_QTMAIN TRUE)
ENDIF (WIN32)

## QtDBus 
FIND_PACKAGE(Qt4 COMPONENTS QtGui QtCore QtWebKit phonon QtXml QtXmlPatterns QtNetwork QtSql QtSvg QtOpenGL REQUIRED)
INCLUDE(${QT_USE_FILE})  # include UseQt4.cmake

#ADD_DEFINITIONS(-DQT_NO_KEYWORDS) # corresponds to CONFIG += no_keywords

set(Boost_USE_STATIC_LIBS   ON)
set(Boost_USE_MULTITHREADED ON)

IF (APPLE)
  find_package(Boost 1.44 REQUIRED COMPONENTS system signals filesystem iostreams program_options)
ELSE ()
  find_package(Boost 1.44 REQUIRED COMPONENTS system signals filesystem iostreams program_options zlib)
ENDIF ()
include_directories (${Boost_INCLUDE_DIRS})

SET(QTPROPERTYBROWSER_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/extern/qtpropertybrowser-2.5_1/src)
SET(QTPROPERTYBROWSER_LIBRARY_DIRS ${PROJECT_SOURCE_DIR}/extern/qtpropertybrowser-2.5_1/src)
SET(PVIZMODEL_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/pvizmodel/src)
##SET(NBCLIENT_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/extern/nbclient/include)

IF (PVIZMODEL_USE_HDF5)
  ADD_DEFINITIONS(-DPVIZMODEL_USE_HDF5)
ENDIF (PVIZMODEL_USE_HDF5)

if (PVIZRPC_CLIENT_ENABLE)
  add_definitions(-DPVIZRPC_CLIENT_ENABLE)

  ## Protobuf
  find_path(Protobuf_INCLUDE_DIR google/protobuf/service.h $ENV{PROTOBUF_HOME}/include)
  find_library(Protobuf_LIBRARIES NAMES protobuf PATHS $ENV{PROTOBUF_HOME}/lib)
  include_directories(${Protobuf_INCLUDE_DIR})
  
  ## Protorpc
  find_path(Protorpc_INCLUDE_DIR protorpc.pb.h $ENV{PROTORPC_HOME}/include)
  find_library(Protorpc_LIBRARIES NAMES protorpc PATHS $ENV{PROTORPC_HOME}/lib)
  include_directories(${Protorpc_INCLUDE_DIR})

  ## Boost
  find_package(Boost 1.44 COMPONENTS system signals)
  include_directories (${Boost_INCLUDE_DIRS})

  ## PvizRPC
  #set(Pvizrpc_BASENAME pvizrpc)
  #set(Pvizrpc_OUTFILES ${Pvizrpc_BASENAME}.pb.h ${Pvizrpc_BASENAME}.pb.cc)
  
  include_directories (${PROJECT_BINARY_DIR}/pvizmodel/src)
  ##list(APPEND ${CMAKE_PROJECT_NAME}_SRCS ${Pvizrpc_OUTFILES})
endif(PVIZRPC_CLIENT_ENABLE)

#-----------------------------------------------------------------------------
# Option to enable ACTIVEMQ_CLIENT_ENABLE
#-----------------------------------------------------------------------------
option (ACTIVEMQ_CLIENT_ENABLE "Enable ActiveMQ Server" OFF)
if (ACTIVEMQ_CLIENT_ENABLE)
  add_definitions(-DDAGTM_ACTIVEMQ_SERVER_ON)
endif()

IF (APPLE)
  add_definitions(-DDARWIN)
  add_definitions(-DSIGPROCMASK_SETS_THREAD_MASK)

  ## Activemq-CPP
  find_path(Activemqcpp_INCLUDE_DIR activemq/library/ActiveMQCPP.h $ENV{ACTIVEMQCPP_HOME}/include/activemq-cpp-3.4.0)
  find_library(Activemqcpp_LIBRARIES NAMES activemq-cpp PATHS $ENV{ACTIVEMQCPP_HOME}/lib)
  include_directories(${Activemqcpp_INCLUDE_DIR})
  message(Activemqcpp_INCLUDE_DIR = ${Activemqcpp_INCLUDE_DIR})
  message(Activemqcpp_LIBRARIES = ${Activemqcpp_LIBRARIES})

  ## APR
  find_path(Apr_INCLUDE_DIR apr.h $ENV{APR_HOME}/include/apr-1)
  find_library(Apr_LIBRARIES NAMES apr-1 PATHS $ENV{APR_HOME}/lib)
  message(Apr_INCLUDE_DIR = ${Apr_INCLUDE_DIR})
  message(Apr_LIBRARIES = ${Apr_LIBRARIES})
  include_directories(${Apr_INCLUDE_DIR})

  ## APR-UTIL
  find_path(Apu_INCLUDE_DIR apu.h $ENV{APU_HOME}/include/apr-1)
  find_library(Apu_LIBRARIES NAMES aprutil-1 PATHS $ENV{APU_HOME}/lib)
  message(Apu_INCLUDE_DIR = ${Apu_INCLUDE_DIR})
  message(Apu_LIBRARIES = ${Apu_LIBRARIES})
  include_directories(${Apu_INCLUDE_DIR})
ELSEIF (WIN32)
  add_definitions(-DWIN32)
  add_definitions(-DWIN32_LEAN_AND_MEAN)
  add_definitions(-DNDEBUG)
  add_definitions(-D_CONSOLE)
  add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
  add_definitions(-DNOMINMAX)
  add_definitions(-DHAVE_WINSOCK2_H)
  add_definitions(-DCMS_DLL)
  add_definitions(-DAMQCPP_DLL)
  add_definitions(-DDECAF_DLL)

  ## Activemq-CPP
  find_path(Activemqcpp_INCLUDE_DIR activemq/library/ActiveMQCPP.h $ENV{ACTIVEMQCPP_HOME}/include)
  find_library(Activemqcpp_LIBRARIES NAMES activemq-cpp PATHS $ENV{ACTIVEMQCPP_HOME}/lib)
  
  message(ACTIVEMQCPP_HOME = $ENV{ACTIVEMQCPP_HOME})
  message(Activemqcpp_INCLUDE_DIR = ${Activemqcpp_INCLUDE_DIR})
  message(Activemqcpp_LIBRARIES = ${Activemqcpp_LIBRARIES})
  include_directories(${Activemqcpp_INCLUDE_DIR})
  
  ## APR
  find_path(Apr_INCLUDE_DIR apr.h $ENV{APR_HOME}/include)
  find_library(Apr_LIBRARIES NAMES apr-1 PATHS $ENV{APR_HOME}/lib)
  message(Apr_INCLUDE_DIR = ${Apr_INCLUDE_DIR})
  message(Apr_LIBRARIES = ${Apr_LIBRARIES})
  include_directories(${Apr_INCLUDE_DIR})

  ## APR-UTIL
  find_path(Apu_INCLUDE_DIR apu.h $ENV{APR_HOME}/include)
  find_library(Apu_LIBRARIES NAMES aprutil-1 PATHS $ENV{APR_HOME}/lib)
  ##find_library(Apriconv_LIBRARIES NAMES apriconv-1 PATHS $ENV{APR_HOME}/lib)
  message(Apu_INCLUDE_DIR = ${Apu_INCLUDE_DIR})
  message(Apu_LIBRARIES = ${Apu_LIBRARIES})
  include_directories(${Apu_INCLUDE_DIR})

ENDIF()

IF (APPLE)
  ## Protobuf
  find_path(Protobuf_INCLUDE_DIR google/protobuf/service.h $ENV{PROTOBUF_HOME}/include)
  SET(CMAKE_FIND_LIBRARY_PREFIXES )
  find_library(Protobuf_LIBRARIES NAMES libprotobuf.a protobuf PATHS $ENV{PROTOBUF_HOME}/lib)
  include_directories(${Protobuf_INCLUDE_DIR})
  message(Protobuf_INCLUDE_DIR = ${Protobuf_INCLUDE_DIR})
  message(Protobuf_LIBRARIES = ${Protobuf_LIBRARIES})
ENDIF()

IF (WIN32)
  ## Protobuf
  find_path(Protobuf_INCLUDE_DIR google/protobuf/service.h $ENV{PROTOBUF_HOME}/include)
  find_library(Protobuf_LIBRARIES NAMES libprotobuf PATHS $ENV{PROTOBUF_HOME}/lib)
  message(Protobuf_INCLUDE_DIR = ${Protobuf_INCLUDE_DIR})
  message(Protobuf_LIBRARIES = ${Protobuf_LIBRARIES})
  include_directories(${Protobuf_INCLUDE_DIR})
ENDIF()

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
  vtkCubeAxes2Actor.h vtkCubeAxes2Actor.cxx
  pvizExtractSelectedPolyDataIds.h pvizExtractSelectedPolyDataIds.cxx
  #PvizPoint.cpp PvizPoint.h
  #PvizCluster.cpp PvizCluster.h
  #PvizModel.cpp PvizModel.h
  aboutdialog.cpp
  helpdialog.cpp
  #message.pb.cc message.pb.h
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

QT4_WRAP_UI(UI_SRCS ${UIS})
QT4_WRAP_CPP(MOC_SRCS ${HDRS})
QT4_ADD_RESOURCES(RC_SRCS ${RSCS})

IF(Q_WS_WIN)
  SET(SRCS ${SRCS} CMakeSetup.rc)
ENDIF(Q_WS_WIN)

IF(Q_WS_MAC)
  SET(SRCS ${SRCS} Icons/pviz.icns)
  SET(MACOSX_BUNDLE_ICON_FILE pviz.icns)
  SET(MACOSX_BUNDLE_BUNDLE_NAME PlotViz)
  SET_SOURCE_FILES_PROPERTIES(Icons/pviz.icns PROPERTIES 
    MACOSX_PACKAGE_LOCATION Resources)
ENDIF(Q_WS_MAC)

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
  QVTK 
  QtSolutions_PropertyBrowser
  pvizmodel
  ##nbclient
  ${QT_LIBRARIES}
  ${Boost_LIBRARIES} 
  ${Protobuf_LIBRARIES}
  )

IF (APPLE)
  target_link_libraries (pviz3 
    /opt/pviz3dev/lib/libactivemq-cpp.a
    /opt/local/lib/libapr-1.a
    /opt/local/lib/libaprutil-1.a
    /opt/local/lib/db46/libdb-4.6.a
    /opt/local/lib/libexpat.a
    /opt/local/lib/libiconv.a
    -lssl
    -lcrypto
    -lpthread
    -lz
    )
ENDIF()

IF (WIN32)
  ## Console mode
  target_link_libraries (pviz3 
    ##-subsystem:console
    )
  target_link_libraries (pviz3 
    ${Activemqcpp_LIBRARIES}
	${Apr_LIBRARIES}
	${Apu_LIBRARIES}
	##${Apriconv_LIBRARIES}
	odbc32.lib
	odbccp32.lib
    )
ENDIF()

INSTALL(TARGETS pviz3
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  BUNDLE DESTINATION .
  )

IF (WIN32)
  IF (CMAKE_CXX_SIZEOF_DATA_PTR MATCHES "8")
    SET(WIN_REDIST_DIR "C:/Program Files (x86)/Microsoft Visual Studio 10.0/VC/redist/x64")
  ELSE ()
    SET(WIN_REDIST_DIR "C:/Program Files (x86)/Microsoft Visual Studio 10.0/VC/redist/x86")
  ENDIF ()
  FILE(GLOB QT_REDIST_FILES ${QT_LIBRARY_DIR}/*.dll)
  FILE(GLOB VTK_REDIST_FILES ${VTK_RUNTIME_LIBRARY_DIRS}/*.dll)
  message(VTK_RUNTIME_LIBRARY_DIRS ${VTK_RUNTIME_LIBRARY_DIRS})
  FILE(GLOB WIN_REDIST_FILES ${WIN_REDIST_DIR}/Microsoft.VC100.CRT/*.dll ${WIN_REDIST_DIR}/Microsoft.VC100.OpenMP/*.dll)
  INSTALL(FILES ${QT_REDIST_FILES} DESTINATION bin)
  INSTALL(FILES ${VTK_REDIST_FILES} DESTINATION bin)
  INSTALL(FILES ${WIN_REDIST_FILES} DESTINATION bin)

IF (USE_ACTIVEMQ)  
  ## Activemq-CPP
  FILE(TO_CMAKE_PATH $ENV{ACTIVEMQCPP_HOME}/bin Activemqcpp_REDIST_DIR)
  FILE(GLOB Activemq_REDIST_FILES ${Activemqcpp_REDIST_DIR}/*.dll)
  INSTALL(FILES ${Activemq_REDIST_FILES} DESTINATION bin)
  message(Activemq_REDIST_FILES ${Activemq_REDIST_FILES})

  FILE(TO_CMAKE_PATH $ENV{APR_HOME}/bin Apr_REDIST_DIR)
  FILE(GLOB Apr_REDIST_FILES ${Apr_REDIST_DIR}/*.dll)
  INSTALL(FILES ${Apr_REDIST_FILES} DESTINATION bin) 
  message(Apr_REDIST_FILES ${Apr_REDIST_FILES})
ENDIF ()
ENDIF (WIN32)

IF (APPLE)
  FILE(GLOB VTK_REDIST_FILES ${VTK_LIBRARY_DIRS}/*.dylib)
  SET(VTK_REDIST_DIR pviz3.app/Contents/Libraries)
  SET(QT_REDIST_DIR pviz3.app/Contents/Frameworks)
  SET(QT_REDIST_PLUGINS_DIR pviz3.app/Contents/Plugins)
  SET(QT_REDIST_RESOURCES_DIR pviz3.app/Contents/Resources)
  INSTALL(FILES ${VTK_REDIST_FILES} DESTINATION ${VTK_REDIST_DIR})

  message(QT_LIBRARIES ${QT_LIBRARIES})
  FOREACH(module ${QT_LIBRARIES})
    INSTALL(DIRECTORY ${module} DESTINATION ${QT_REDIST_DIR}
      PATTERN "*debug*" EXCLUDE)
  ENDFOREACH(module)

  FOREACH(plugin imageformats sqldrivers)
    INSTALL(DIRECTORY ${QT_PLUGINS_DIR}/${plugin} DESTINATION ${QT_REDIST_PLUGINS_DIR})
  ENDFOREACH(plugin)
  
  INSTALL(FILES ${PROJECT_SOURCE_DIR}/etc/qt.conf DESTINATION ${QT_REDIST_RESOURCES_DIR})

  FOREACH(file
      /opt/intel/composerxe/lib/libiomp5.dylib
      ${Protobuf_LIBRARIES}
      ##/opt/local/lib/libapr-1.dylib
      ##/opt/local/lib/libaprutil-1.dylib
      ##/opt/local/lib/db46/libdb-4.6.dylib
      ##/opt/local/lib/libexpat.dylib
      ##/opt/local/lib/libiconv.dylib
      ##/opt/pviz3dev/lib/libactivemq-cpp.14.dylib
      )
    INSTALL(FILES ${file} DESTINATION ${VTK_REDIST_DIR})
    IF (IS_SYMLINK ${file})
      get_filename_component(realfile ${file} REALPATH)
      INSTALL(FILES ${realfile} DESTINATION ${VTK_REDIST_DIR})
    ENDIF ()
  ENDFOREACH(file)

  INSTALL(CODE "EXECUTE_PROCESS(COMMAND ${PROJECT_SOURCE_DIR}/etc/install_name.sh)")
  INSTALL(CODE "EXECUTE_PROCESS(COMMAND chmod -R g+rX,o+rX ${CMAKE_INSTALL_PREFIX}/pviz3.app)")
  INSTALL(CODE "EXECUTE_PROCESS(COMMAND find ${CMAKE_INSTALL_PREFIX}/pviz3.app -type l -exec chmod -h g+rX,o+rX {} \;)")
  INSTALL(CODE "EXECUTE_PROCESS(COMMAND chmod -R g+rX,o+rX _CPack_Packages)")
  INSTALL(CODE "EXECUTE_PROCESS(COMMAND find _CPack_Packages -type l -exec chmod -h g+rX,o+rX {} \;)")
ENDIF (APPLE)

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "PlotViz")
SET(CPACK_PACKAGE_VENDOR "SALSA")
#SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/ReadMe.txt")
SET(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE.txt")
SET(CPACK_PACKAGE_VERSION_MAJOR ${PVIZ3_VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${PVIZ3_VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${PVIZ3_VERSION_PATCH})
SET(PVIZ3_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")

#SET(CPACK_STRIP_FILES "bin/pviz3")
#SET(CPACK_SOURCE_STRIP_FILES "")

IF(WIN32 AND NOT UNIX)
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

  #ELSEIF (APPLE)
ELSE()
  SET(CPACK_GENERATOR "PackageMaker")
  SET(CPACK_PACKAGING_INSTALL_PREFIX "/")
  SET(CPACK_PACKAGE_DEFAULT_LOCATION "/Applications")
  SET(CPACK_PACKAGE_INSTALL_DIRECTORY "PVIZ3")
  SET(CPACK_PACKAGE_ICON "${PROJECT_SOURCE_DIR}/src/Icons/pviz.icns")
  SET(CPACK_STRIP_FILES "bin/pviz3")
  SET(CPACK_SOURCE_STRIP_FILES "")
ENDIF()
SET(CPACK_PACKAGE_EXECUTABLES pviz3 Pviz3)
SET(CPACK_BINARY_DRAGNDROP ON)
INCLUDE(CPack)

## OpenMP setting
IF(WIN32 AND NOT UNIX)
  ##SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /openmp")
  ##SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /openmp")
ELSE()
  ##SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp -openmp")
  ##SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopenmp -openmp")
  SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -F/Library/Frameworks")
ENDIF()

##FIND_PACKAGE(Subversion)
##IF(SUBVERSION_FOUND)
##  Subversion_WC_INFO(${PROJECT_SOURCE_DIR} PVIZ3)
##  SET(PVIZ3_REVISION ${PVIZ3_WC_REVISION})
##  MESSAGE("Current revision is ${PVIZ3_WC_REVISION}")
##  ##MESSAGE(PVIZ3_REVISION ${PVIZ3_REVISION})
##  ##Subversion_WC_LOG(${PROJECT_SOURCE_DIR} PVIZ3)
##  ##MESSAGE("Last changed log is ${PVIZ3_LAST_CHANGED_LOG}")
##ELSE()
##  SET(PVIZ3_REVISION 0000)
##ENDIF(SUBVERSION_FOUND)
SET(PVIZ3_REVISION 0000)

##message(CMAKE_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/version.h.in ${CMAKE_CURRENT_BINARY_DIR}/version.h)
##ADD_CUSTOM_TARGET(GenerateHeader
##COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/Generate.cmake
##WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
##)
###DEPENDS pviz3)
##ADD_DEPENDENCIES(pviz3 GenerateHeader)

