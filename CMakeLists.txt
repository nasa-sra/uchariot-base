cmake_minimum_required(VERSION 3.18)

project(UChariot-Base)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "-O2 -pthread")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set (CMAKE_CXX_STANDARD 17)

if(NOT ${CMAKE_CROSSCOMPILING})
    if (NOT ${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm")
        if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
            add_compile_definitions(SIMULATION=true)
        endif()
    endif()
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
    src/Serial.cpp
    src/nmea/context.c
    src/nmea/generate.c
    src/nmea/generator.c
    src/nmea/gmath.c
    src/nmea/info.c
    src/nmea/parse.c
    src/nmea/parser.c
    src/nmea/sentence.c
    src/nmea/time.c
    src/nmea/tok.c
)

set(LIBS )

if (${CMAKE_CROSSCOMPILING})
    list(APPEND SRCS src/subsystems/IMU.cpp)

    find_library(WIRING_PI_LIB libwiringPi.so )
    find_library(WIRING_PI_DEV_LIB libwiringPiDev.so )
    find_library(CRYPT_LIB libcrypt.so.1.1.0)
    list(APPEND LIBS ${WIRING_PI_LIB} ${WIRING_PI_DEV_LIB} ${CRYPT_LIB})

endif()

add_executable(uChariotBase ${SRCS})
target_link_libraries( uChariotBase ${LIBS})
target_include_directories( uChariotBase PUBLIC ./include/)
