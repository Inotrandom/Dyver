cmake_minimum_required(VERSION 3.16)

project(DyverInstaller)

add_compile_options(-Wpedantic -Wall -Wextra)

add_executable(
    ${PROJECT_NAME}
    ${CMAKE_SOURCE_DIR}/installer/installer.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/include)