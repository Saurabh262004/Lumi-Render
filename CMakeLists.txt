cmake_minimum_required(VERSION 3.12)
project(GLib LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(glfw3 REQUIRED)
find_package(assimp REQUIRED)
find_package(OpenGL REQUIRED)

# Detect whether Lumi-render is the top-level project, or is being pulled in
# as a dependency by another repo.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
	set(LUMI_IS_TOP_LEVEL TRUE)
else()
	set(LUMI_IS_TOP_LEVEL FALSE)
endif()

# Library

file(GLOB_RECURSE LIB_SOURCES CONFIGURE_DEPENDS
	"src/*.cpp"
	"src/*.c"
	"external/glad/glad.c"
)

add_library(lumi_render STATIC ${LIB_SOURCES})

target_include_directories(lumi_render
	PUBLIC
		include
		external
)

target_link_libraries(lumi_render
	PUBLIC
		glfw
		OpenGL::GL
		assimp
)

target_compile_options(lumi_render PRIVATE
	$<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra>
)

# Example application
# Only built when Lumi-render is the top-level project

option(LUMI_BUILD_EXAMPLE "Build example app" ${LUMI_IS_TOP_LEVEL})

if(LUMI_BUILD_EXAMPLE)
	add_executable(app
		example/Main.cpp
	)

	target_link_libraries(app
		PRIVATE
			lumi_render
	)
endif()

# Test application (uses local testAssets/, off by default)

option(LUMI_BUILD_TESTAPP "Build testApp" OFF)

if(LUMI_BUILD_TESTAPP)
	add_executable(testApp
		tests/Main.cpp
	)

	target_link_libraries(testApp
		PRIVATE
			lumi_render
	)
endif()
