cmake_minimum_required(VERSION 3.18)

project(UChariot-Base)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "-O2")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SRCS
    src/main.cpp
    src/CanConnection.cpp
    src/Robot.cpp
    src/VescController.cpp
    src/Utils.cpp
    src/NetworkingManager.cpp
)

# set(THREADS_PREFER_PTHREAD_FLAG ON)
# find_package(Threads REQUIRED)

add_executable(uChariotBase ${SRCS})
target_link_libraries( uChariotBase ) # Threads::Threads)
target_include_directories( uChariotBase PUBLIC ./include/)
