cmake_minimum_required(VERSION 3.25.0)
project(dyver)

add_executable(
	${PROJECT_NAME}
	${CMAKE_SOURCE_DIR}/src/main.c
)

add_subdirectory(
	${CMAKE_SOURCE_DIR}/robot
)