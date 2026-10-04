cmake_minimum_required(VERSION 3.25.0)

project(dyver_robot)

add_executable(
	${PROJECT_NAME}
	${CMAKE_SOURCE_DIR}/robot/src/main.c
)