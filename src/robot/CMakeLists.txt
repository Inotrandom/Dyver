cmake_minimum_required(VERSION 3.16)

project(DyverRobot VERSION 0.0.0 LANGUAGES CXX)

add_compile_options(-Wpedantic -Wall -Wextra)
set(CMAKE_CXX_STANDARD 20)

add_executable(
    ${PROJECT_NAME}
    ${CMAKE_SOURCE_DIR}/src/robot/robot.cpp
)

message(${wiringpi_SOURCE_DIR}/wiringPi)

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
    ${wiringpi_SOURCE_DIR}/wiringPi
)