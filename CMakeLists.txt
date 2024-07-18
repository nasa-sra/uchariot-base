cmake_minimum_required(VERSION 3.16)

project(UChariot-Base)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -pthread -std=c++17")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set (CMAKE_CXX_STANDARD 17)

# cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain.cmake ..

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
    src/subsystems/GPS.cpp
)

set(LIBS)
find_package(Eigen3 CONFIG REQUIRED)
find_library(I2C_LIB libi2c.so)
find_library(GPS_LIB libgps.so)
find_library(DBUS_LIB libdbus-1.so.3)
find_library(SYSTEMD_LIB libsystemd.so.0)
find_library(CAP_LIB libcap.so.2)
find_library(GCRYPT_LIB libgcrypt.so.20)
find_library(LZMA_LIB liblzma.so.5)
find_library(ZSTD_LIB libzstd.so.1)
find_library(LZ4_LIB liblz4.so.1)
find_library(GPG_ERROR_LIB libgpg-error.so.0)
find_library(RT_LIB librt.so.1)
list(APPEND LIBS Eigen3::Eigen ${I2C_LIB} ${GPS_LIB} ${DBUS_LIB} ${SYSTEMD_LIB} ${CAP_LIB} ${GCRYPT_LIB} ${LZMA_LIB} ${ZSTD_LIB} ${LZ4_LIB} ${GPG_ERROR_LIB} ${RT_LIB})

add_executable(uChariotBase ${SRCS})
target_link_libraries( uChariotBase ${LIBS})
target_include_directories( uChariotBase PUBLIC ./include/)
