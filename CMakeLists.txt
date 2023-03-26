cmake_minimum_required(VERSION 3.18.1)

if (NOT CMAKE_TOOLCHAIN_FILE AND WIN32)
	find_path(VCPKG_ROOT NAMES "vcpkg.exe" HINTS "C:\dev")
	if (VCPKG_ROOT)
		set(CMAKE_TOOLCHAIN_FILE ${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)
	endif()
endif()

project(Hello CXX)

find_package(Boost REQUIRED COMPONENTS json)

add_subdirectory(src/gltf++)
add_subdirectory(tests)
