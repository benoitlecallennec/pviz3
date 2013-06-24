
set(${CMAKE_PROJECT_NAME}_SRCS 
  pvizcomm.cpp pvizcomm.h
  pvizmodel.cpp pvizmodel.h
  pvizpoint.cpp pvizpoint.h
  pvizcluster.cpp pvizcluster.h
  pvizline.cpp pvizline.h
  pvizutil.h
  ##pvizmessage.pb.cc pvizmessage.pb.h
)

## Boost
set(Boost_USE_STATIC_LIBS   ON)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost 1.44 COMPONENTS system signals filesystem iostreams zlib)
include_directories (${Boost_INCLUDE_DIRS})

#-----------------------------------------------------------------------------
# Option to enable HDF5
#-----------------------------------------------------------------------------
option (PVIZMODEL_USE_HDF5 "Enable HDF5" OFF)
if (PVIZMODEL_USE_HDF5)
  add_definitions(-DPVIZMODEL_USE_HDF5)
  ## HDF5
  find_package(HDF5 REQUIRED)
  include_directories (${HDF5_INCLUDE_DIRS} ${HDF5_INCLUDE_DIRS}/cpp)
  
  ## MPI
  find_package(MPI REQUIRED)
  include_directories (${MPI_INCLUDE_PATH})

  ## GSL
  set(GSL_INCLUDE_DIRS $ENV{GSL_HOME}/include)
  set(GSL_LIBRARY_DIRS $ENV{GSL_HOME}/lib)
  set(GSL_LIBRARIES gsl gslcblas)
  link_directories(${GSL_LIBRARY_DIRS})
  include_directories(${GSL_INCLUDE_DIRS})
endif(PVIZMODEL_USE_HDF5)

#-----------------------------------------------------------------------------
# Option to enable PvizRPC client 
#-----------------------------------------------------------------------------
option (PVIZRPC_CLIENT_ENABLE "Enable PvizRPC client" OFF)
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
  find_package(Boost 1.44 COMPONENTS system signals filesystem iostreams zlib)
  include_directories (${Boost_INCLUDE_DIRS})

  ## PvizRPC
  set(Pvizrpc_BASENAME pvizrpc)
  set(Pvizrpc_OUTFILES ${Pvizrpc_BASENAME}.pb.h ${Pvizrpc_BASENAME}.pb.cc)
  
  add_custom_command(
    OUTPUT ${Pvizrpc_OUTFILES}
    COMMAND protoc ${Pvizrpc_BASENAME}.proto --cpp_out=${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS
    ${Pvizrpc_BASENAME}.proto
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  )
  include_directories (${CMAKE_CURRENT_BINARY_DIR})
  list(APPEND ${CMAKE_PROJECT_NAME}_SRCS ${Pvizrpc_OUTFILES})
endif(PVIZRPC_CLIENT_ENABLE)

IF (APPLE)
  ## Protobuf
  find_path(Protobuf_INCLUDE_DIR google/protobuf/service.h $ENV{PROTOBUF_HOME}/include)
  find_library(Protobuf_LIBRARIES NAMES protobuf PATHS $ENV{PROTOBUF_HOME}/lib)
  include_directories(${Protobuf_INCLUDE_DIR})
ENDIF()

IF (WIN32)
  ## Protobuf
  find_path(Protobuf_INCLUDE_DIR google/protobuf/service.h $ENV{PROTOBUF_HOME}/include)
  find_library(Protobuf_LIBRARIES NAMES libprotobuf PATHS $ENV{PROTOBUF_HOME}/lib)
  message(Protobuf_INCLUDE_DIR = ${Protobuf_INCLUDE_DIR})
  message(Protobuf_LIBRARIES = ${Protobuf_LIBRARIES})
  include_directories(${Protobuf_INCLUDE_DIR})
ENDIF()

# SHARED doesn't work in Windows
add_library (pvizmodel STATIC 
  ${${CMAKE_PROJECT_NAME}_SRCS}
)

if (PVIZMODEL_USE_HDF5)
target_link_libraries(pvizmodel 
  hdf5_cpp
  ${HDF5_LIBRARIES}
  ${MPI_LIBRARIES}
  ${GSL_LIBRARIES}
)
endif(PVIZMODEL_USE_HDF5)

if (PVIZRPC_CLIENT_ENABLE)
  target_link_libraries (pvizmodel ${Protobuf_LIBRARIES})
  target_link_libraries (pvizmodel ${Protorpc_LIBRARIES})
  target_link_libraries (pvizmodel ${Boost_LIBRARIES})
endif(PVIZRPC_CLIENT_ENABLE)

target_link_libraries (pvizmodel 
  ${Protobuf_LIBRARIES}
  ${Boost_LIBRARIES}
  )
