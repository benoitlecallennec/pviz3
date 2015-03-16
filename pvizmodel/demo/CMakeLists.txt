# Make sure the compiler can find include files from our Hello library.
include_directories (${PVIZMODEL_SOURCE_DIR}/src)

# Make sure the linker can find the Hello library once it is built.
link_directories (${PVIZMODEL_BINARY_DIR}/src)

# Add executable called "helloDemo" that is built from the source files
# "demo.cxx" and "demo_b.cxx".  The extensions are automatically found.
add_executable (pvizmodeldemo demo.cpp)

# Link the executable to the Hello library.
target_link_libraries (pvizmodeldemo pvizmodel)

## Boost
set(Boost_USE_STATIC_LIBS   ON)
set(Boost_USE_MULTITHREADED ON)

IF (APPLE OR UNIX)
  find_package(Boost 1.44 REQUIRED COMPONENTS system signals filesystem iostreams)
  target_link_libraries (pvizmodeldemo z)
ELSE ()
  find_package(Boost 1.44 REQUIRED COMPONENTS system signals filesystem iostreams zlib)
ENDIF ()
include_directories (${Boost_INCLUDE_DIRS})

target_link_libraries (pvizmodeldemo ${Boost_LIBRARIES})

## Protobuf
IF (PVIZRPC_CLIENT_ENABLE)
find_path(Protobuf_INCLUDE_DIR google/protobuf/service.h $ENV{PROTOBUF_HOME}/include)
find_library(Protobuf_LIBRARIES NAMES protobuf PATHS $ENV{PROTOBUF_HOME}/lib)
include_directories(${Protobuf_INCLUDE_DIR})
ENDIF ()


