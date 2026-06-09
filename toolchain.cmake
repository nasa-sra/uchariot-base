message("Cross compiling for pi")
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_GENERATOR "Unix Makefiles")

#TODO: add spdlog into cmake file with proper fetches

set(UCHARIOT_TOOLCHAIN_PREFIX "aarch64-linux-gnu" CACHE STRING "Compiler prefix for the target toolchain")
set(UCHARIOT_SYSROOT "$ENV{UCHARIOT_SYSROOT}" CACHE PATH "Path to the target sysroot")

#TODO: log files being set up 
set(_UCHARIOT_SYSROOT_CANDIDATES
	"${UCHARIOT_SYSROOT}"
	"/opt/uchariot-sys/sysroot"
	"/usr/local/uchariot-sys/sysroot"
)

#TODO: log if any target roots are found or not
foreach(_candidate_sysroot IN LISTS _UCHARIOT_SYSROOT_CANDIDATES)
	if(_candidate_sysroot AND EXISTS "${_candidate_sysroot}")
		set(CMAKE_SYSROOT "${_candidate_sysroot}" CACHE PATH "Target sysroot" FORCE)
		break()
	endif()
endforeach()

#TODO: Log system root availability
if(CMAKE_SYSROOT)
	set(CMAKE_FIND_ROOT_PATH "${CMAKE_SYSROOT}")
endif()

set(CMAKE_BUILD_RPATH /usr/lib/aarch64-linux-gnu)

set(CMAKE_C_COMPILER "/usr/bin/${UCHARIOT_TOOLCHAIN_PREFIX}-gcc" CACHE FILEPATH "C compiler" FORCE)
set(CMAKE_CXX_COMPILER "/usr/bin/${UCHARIOT_TOOLCHAIN_PREFIX}-g++" CACHE FILEPATH "C++ compiler" FORCE)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(CMAKE_CROSSCOMPILING true)

# set(OpenCV_DIR ${CMAKE_SYSROOT}/usr/local/lib/cmake/opencv4/)
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_SYSROOT}/usr/lib/aarch64-linux-gnu)