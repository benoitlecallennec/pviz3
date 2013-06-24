cmake_minimum_required(VERSION 2.8)

PROJECT(NBClient)

set(NBCLIENT_HDR
    include/Callback.h
    include/ClientCall.h
    include/Connection.h
    include/DistributionTrace.h
    include/NBEntity.h
    include/NBEvent.h
    include/NBEventGenerator.h
    include/NBEventHeaders.h
    include/NBEventID.h
    include/NBEventIDGenerator.h
    include/Profile.h
    include/ROOTCallback.h
    include/ReceiverThread.h
    include/ServiceClient.h
    include/ServiceException.h
    include/TCServerUIModel.h
    include/TCall.h
    include/TCallback.h
    include/TClarensServer.h
    include/TFileCanvasManager.h
    include/TRootletHelper.h
    include/TServiceClient.h
    include/TUtils.h
    include/Thread.h
    include/Utils.h
    include/constants.h
    include/nbcppdll.h
    include/stdafx.h
    include/targetver.h
)

set(NBCLIENT_SRC 
    src/Callback.cpp
    src/Connection.cpp
    src/NBEntity.cpp
    src/NBEvent.cpp
    src/NBEventGenerator.cpp
    src/NBEventHeaders.cpp
    src/NBEventID.cpp
    src/NBEventIDGenerator.cpp
    src/Profile.cpp
    src/ReceiverThread.cpp
    src/ServiceClient.cpp
    src/ServiceException.cpp
    src/Thread.cpp
    src/ClientCall.cpp
    src/DistributionTrace.cpp
    src/Utils.cpp
)

include_directories(${PROJECT_SOURCE_DIR}/include)

add_library(nbclient 
    ${NBCLIENT_HDR}
    ${NBCLIENT_SRC}
)

