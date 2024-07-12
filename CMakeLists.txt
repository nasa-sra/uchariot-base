cmake_minimum_required(VERSION 3.18)

project(UChariot-Base)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "-O2 -pthread")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set (CMAKE_CXX_STANDARD 17)

if(NOT ${CMAKE_CROSSCOMPILING})
    add_compile_definitions(SIMULATION=true)
    message("Compiling for simulation")
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
    src/StateReporter.cpp
    src/controllers/TeleopController.cpp
    src/subsystems/DriveBase.cpp
    src/subsystems/GPS.cpp
)

set(LIBS)
find_package(Eigen3 CONFIG REQUIRED)
find_library(GPS_LIB libgps.so)
find_library(DBUS_LIB libdbus-1.so.3)
find_library(SYSTEMD_LIB libsystemd.so.0)
find_library(CAP_LIB libcap.so.2)
find_library(GCRYPT_LIB libgcrypt.so.20)
find_library(LZMA_LIB liblzma.so.5)
find_library(ZSTD_LIB libzstd.so.1)
find_library(LZ4_LIB liblz4.so.1)
find_library(GPG_ERROR_LIB libgpg-error.so.0)
list(APPEND LIBS Eigen3::Eigen ${GPS_LIB} ${DBUS_LIB} ${SYSTEMD_LIB} ${CAP_LIB} ${GCRYPT_LIB} ${LZMA_LIB} ${ZSTD_LIB} ${LZ4_LIB} ${GPG_ERROR_LIB})

if (${CMAKE_CROSSCOMPILING})
    list(APPEND SRCS src/subsystems/LSM6DSOX.cpp)
    list(APPEND SRCS src/subsystems/BNO055.cpp)

    find_library(WIRING_PI_LIB libwiringPi.so )
    find_library(WIRING_PI_DEV_LIB libwiringPiDev.so )
    find_library(CRYPT_LIB libcrypt.so.1.1.0)
    list(APPEND LIBS ${WIRING_PI_LIB} ${WIRING_PI_DEV_LIB} ${CRYPT_LIB})

endif()

add_executable(uChariotBase ${SRCS})
target_link_libraries( uChariotBase ${LIBS})
target_include_directories( uChariotBase PUBLIC ./include/)
