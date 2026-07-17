cmake_minimum_required(VERSION 3.16)

project(UChariot-Base)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -pthread -std=c++17")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set (CMAKE_CXX_STANDARD 17)
# cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain.cmake ..

set(SRCS
    src/main.cpp
    src/CanConnection.cpp
    src/Robot.cpp
    src/VescController.cpp
    src/Utils.cpp
    src/NetworkManager.cpp
    src/StateReporter.cpp
    src/MessageQueue.cpp

    src/controllers/OverrideController.cpp
    src/controllers/TeleopController.cpp
    src/controllers/PathingController.cpp
    src/controllers/FollowingController.cpp
    src/controllers/SummonController.cpp

    src/subsystems/DriveBase.cpp
    src/subsystems/Localization.cpp
    src/subsystems/Vision.cpp
    src/subsystems/BNO055.cpp
    src/subsystems/GPS.cpp
    src/subsystems/INA228.cpp
    
    src/tinyxml2.cpp
    src/pathgen/Bezier.cpp
    src/pathgen/Curve.cpp
    src/pathgen/PathGenerator.cpp
)

set(LIBS)
set(Eigen3_DIR /usr/share/eigen3/cmake)

find_package(Eigen3 CONFIG REQUIRED)
find_library(I2C_LIB i2c)
if(NOT I2C_LIB)
    set(I2C_LIB i2c)
endif()

find_library(GPS_LIB gps)
if(NOT GPS_LIB)
    set(GPS_LIB gps)
endif()

if (${SIMULATION})
    message("Compiling for simulation")
    add_compile_definitions(SIMULATION=true)
endif()

add_executable(uChariotBase ${SRCS})
target_link_libraries( uChariotBase Eigen3::Eigen ${I2C_LIB} ${GPS_LIB})
target_include_directories( uChariotBase PUBLIC ./include/)
