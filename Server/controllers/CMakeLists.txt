cmake_minimum_required(VERSION 3.10)
project(ArmControlServer)


add_library(ControllersLib
    queryGenerator.cpp
    serverLogic.cpp
    mcuSocket.cpp
    robotInformation.cpp
    controllerInfo.cpp
    dbman.cpp
    srvCore.cpp  srvCore.h
)

target_include_directories(ControllersLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})