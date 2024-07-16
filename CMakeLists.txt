cmake_minimum_required(VERSION 3.16)

project(UChariot-Base)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "-O2 -pthread -isystem ${CMAKE_SYSROOT}/usr/include")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set (CMAKE_CXX_STANDARD 17)

# if(NOT ${CMAKE_CROSSCOMPILING})
#     add_compile_definitions(SIMULATION=true)
#     message("Compiling for simulation")
# endif()

set(SRCS
    src/main.cpp
    src/CanConnection.cpp
    src/Robot.cpp
    src/VescController.cpp
    src/Utils.cpp
    src/NetworkManager.cpp
    src/StateReporter.cpp
    src/controllers/TeleopController.cpp
    src/controllers/PathingController.cpp
    src/subsystems/DriveBase.cpp
    src/subsystems/Localization.cpp
    src/subsystems/Vision.cpp
    src/tinyxml2.cpp
    src/pathgen/Bezier.cpp
    src/pathgen/Curve.cpp
    src/pathgen/PathGenerator.cpp
    src/subsystems/BNO055.cpp
)

set(LIBS)
find_package(Eigen3 CONFIG REQUIRED)
find_library(I2C_LIB libi2c.so)
list(APPEND LIBS Eigen3::Eigen ${I2C_LIB})

add_executable(uChariotBase ${SRCS})
target_link_libraries( uChariotBase ${LIBS})
target_include_directories( uChariotBase PUBLIC ./include/)