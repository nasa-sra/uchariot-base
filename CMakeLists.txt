cmake_minimum_required(VERSION 3.18)

project(UChariot-Base)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "-O2 -pthread -Werror=return-type")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set (CMAKE_CXX_STANDARD 17)

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
    src/tinyxml2.cpp
    src/pathgen/Bezier.cpp
    src/pathgen/Curve.cpp
    src/pathgen/PathGenerator.cpp
)

set(LIBS)

find_package(Eigen3 CONFIG REQUIRED)

list(APPEND LIBS Eigen3::Eigen)

list(APPEND SRCS src/subsystems/BNO055.cpp)

find_library(WIRING_PI_LIB libwiringPi.so )
find_library(WIRING_PI_DEV_LIB libwiringPiDev.so )
find_library(CRYPT_LIB libcrypt.so.1.1.0)
list(APPEND LIBS ${WIRING_PI_LIB} ${WIRING_PI_DEV_LIB} ${CRYPT_LIB})

add_executable(uChariotBase ${SRCS})
target_link_libraries( uChariotBase ${LIBS})
target_include_directories( uChariotBase PUBLIC ./include/)
