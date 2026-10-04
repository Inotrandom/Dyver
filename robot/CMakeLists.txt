cmake_minimum_required(VERSION 3.25.0)

project(dyver_robot)

include(FetchContent)

FetchContent_Declare(
	pigpio
	GIT_REPOSITORY https://github.com/joan2937/pigpio
	GIT_TAG master
	GIT_SHALLOW true
	GIT_PROGRESS true
	USES_TERMINAL_DOWNLOAD true
)

FetchContent_MakeAvailable(pigpio)

add_executable(
	${PROJECT_NAME}
	${CMAKE_SOURCE_DIR}/robot/src/main.c
)

target_include_directories(
	${PROJECT_NAME}
	PRIVATE
	${CMAKE_SOURCE_DIR}/include
	${CMAKE_SOURCE_DIR}/robot/include
	${pigpio_SOURCE_DIR}
)