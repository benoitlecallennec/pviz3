cmake_minimum_required(VERSION 2.8)
 
PROJECT(QTPROPERTYBROWSER)
 
FIND_PACKAGE(Qt4 COMPONENTS QtCore QtGui REQUIRED)
INCLUDE(${QT_USE_FILE})  # include UseQt4.cmake

# support for out-of-source build
INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_BINARY_DIR} #this is where ui_SimpleViewUI.h is generated
  ${CMAKE_CURRENT_SOURCE_DIR}
)

# Set your files and resources here
SET(QTPROPERTYBROWSER_SOURCES
  qtpropertybrowser.cpp
  qtpropertymanager.cpp
  qteditorfactory.cpp
  qtvariantproperty.cpp
  qttreepropertybrowser.cpp
  qtbuttonpropertybrowser.cpp
  qtgroupboxpropertybrowser.cpp
  qtpropertybrowserutils.cpp
)

SET(QTPROPERTYBROWSER_HEADERS
  qtpropertybrowser.h
  qtpropertymanager.h
  qteditorfactory.h
  qtvariantproperty.h
  qttreepropertybrowser.h
  qtbuttonpropertybrowser.h
  qtgroupboxpropertybrowser.h
  qtpropertybrowserutils_p.h
) 

SET(QTPROPERTYBROWSER_RESOURCES
  qtpropertybrowser.qrc
)

QT4_WRAP_CPP(MOCSrcs ${QTPROPERTYBROWSER_HEADERS})

## We need custom outfile names
QT4_GENERATE_MOC(qtpropertymanager.cpp ${CMAKE_CURRENT_BINARY_DIR}/qtpropertymanager.moc)
QT4_GENERATE_MOC(qteditorfactory.cpp ${CMAKE_CURRENT_BINARY_DIR}/qteditorfactory.moc)
QT4_GENERATE_MOC(qttreepropertybrowser.cpp ${CMAKE_CURRENT_BINARY_DIR}/qttreepropertybrowser.moc)

QT4_ADD_RESOURCES(RSCrcs ${QTPROPERTYBROWSER_RESOURCES})
 
SOURCE_GROUP("Generated" FILES
  ${MOCSrcs}
  ${RSCrcs}
)

ADD_CUSTOM_TARGET(generate_moc_files ALL DEPENDS 
  ${MOCSrcs} 
  qtpropertymanager.moc
  qteditorfactory.moc
  qttreepropertybrowser.moc
)
 
ADD_LIBRARY(QtSolutions_PropertyBrowser STATIC
  ${QTPROPERTYBROWSER_SOURCES}
  ${QTPROPERTYBROWSER_HEADERS}
  ${RSCrcs}
  #${QT_LIBRARIES}
  #${QT_QTCORE_LIBRARY}
  #${QT_QTGUI_LIBRARY}
  moc_qtpropertybrowserutils_p.cxx
)

TARGET_LINK_LIBRARIES(QtSolutions_PropertyBrowser
  ${QT_LIBRARIES}
)

ADD_DEPENDENCIES(QtSolutions_PropertyBrowser generate_moc_files)

