cmake_minimum_required(VERSION 3.18)

project(UChariot-Base)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "-O2 -pthread")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set (CMAKE_CXX_STANDARD 17)

if(NOT ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "arm")
    add_compile_definitions(SIMULATION=true)
endif()

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    add_compile_definitions(LINUX=true)
endif()

set(SRCS
    src/main.cpp
    src/CanConnection.cpp
    src/Robot.cpp
    src/VescController.cpp
    src/Utils.cpp
    src/NetworkManager.cpp
    src/controllers/TeleopController.cpp
    src/subsystems/DriveBase.cpp
)

add_executable(uChariotBase ${SRCS})
target_link_libraries( uChariotBase )
target_include_directories( uChariotBase PUBLIC ./include/)