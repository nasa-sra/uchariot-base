cmake_minimum_required(VERSION 3.18)

project(UChariot-Base)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "-O2 -pthread")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set (CMAKE_CXX_STANDARD 17)

# if(NOT ${CMAKE_CROSSCOMPILING})
#     add_compile_definitions(SIMULATION=true)
#     message("Compiling for simulation")
# endif()

add_compile_definitions(SIMULATION=false)


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
    src/subsystems/BNO055.cpp
)

set(LIBS)

find_package(Eigen3 CONFIG REQUIRED)
list(APPEND LIBS Eigen3::Eigen)
list(APPEND LIBS i2c)

if (${CMAKE_CROSSCOMPILING})
    list(APPEND SRCS src/subsystems/LSM6DSOX.cpp)
    # list(APPEND SRCS src/subsystems/BNO055.cpp)

    find_library(WIRING_PI_LIB libwiringPi.so )
    find_library(WIRING_PI_DEV_LIB libwiringPiDev.so )
    find_library(CRYPT_LIB libcrypt.so.1.1.0)
    list(APPEND LIBS ${WIRING_PI_LIB} ${WIRING_PI_DEV_LIB} ${CRYPT_LIB})

endif()

add_executable(uChariotBase ${SRCS})
target_link_libraries( uChariotBase ${LIBS})
target_include_directories( uChariotBase PUBLIC ./include/)
