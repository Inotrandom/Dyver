# This is the build system for Dyver. If you require assistance
# with adding libraries and dependencies, please contact a project manager
# (Most likely Estelle Coonan)

cmake_minimum_required(VERSION 3.16)
project(DyverTopside VERSION 0.0.0 LANGUAGES CXX)

add_compile_options(-Wpedantic -Wall -Wextra)

include(FetchContent)

set(CMAKE_CXX_STANDARD 20)

FetchContent_Declare(
    eigen
    GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
    GIT_TAG 5.0.0
    GIT_PROGRESS TRUE
    GIT_SHALLOW TRUE
    USES_TERMINAL_DOWNLOAD TRUE
)

FetchContent_Declare(
    sdl
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.2.x
    GIT_PROGRESS TRUE
    GIT_SHALLOW TRUE
    USES_TERMINAL_DOWNLOAD TRUE
)

FetchContent_Declare(
    sdl_ttf
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf
    GIT_TAG release-3.2.x
    GIT_PROGRESS TRUE
    GIT_SHALLOW TRUE
    USES_TERMINAL_DOWNLOAD TRUE
)

FetchContent_Declare(
    matplotplusplus
    GIT_REPOSITORY https://github.com/alandefreitas/matplotplusplus.git
    GIT_TAG master
    GIT_PROGRESS TRUE
    GIT_SHALLOW TRUE
    USES_TERMINAL_DOWNLOAD TRUE
)

FetchContent_Declare(
    dss
    GIT_REPOSITORY https://github.com/Inotrandom/deep-sea-shell.git
    GIT_TAG v1.x
    GIT_PROGRESS TRUE
    GIT_SHALLOW TRUE
    USES_TERMINAL_DOWNLOAD TRUE
)

FetchContent_Declare(
    pigpio
    GIT_REPOSITORY https://github.com/joan2937/pigpio
    GIT_TAG master
    GIT_PROGRESS TRUE
    GIT_SHALLOW TRUE
    USES_TERMINAL_DOWNLOAD TRUE
)

FetchContent_MakeAvailable(pigpio)

set(SDLTTF_VENDORED ON)
FetchContent_MakeAvailable(sdl sdl_ttf)
FetchContent_MakeAvailable(eigen)
FetchContent_MakeAvailable(dss)
FetchContent_MakeAvailable(matplotplusplus)

add_executable(
    ${PROJECT_NAME}
    ${CMAKE_SOURCE_DIR}/src/topside/topside.cpp
    ${CMAKE_SOURCE_DIR}/src/topside/core/rov.cpp
    ${CMAKE_SOURCE_DIR}/src/thruster/thruster.cpp
    ${CMAKE_SOURCE_DIR}/src/bindable_action.cpp
    ${CMAKE_SOURCE_DIR}/src/topside/app.cpp
    ${CMAKE_SOURCE_DIR}/include/cli/cli.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/include)

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${eigen_SOURCE_DIR}
    ${dss_SOURCE_DIR}
    ${sdl_SOURCE_DIR}/include
    ${matplotplusplus_SOURCE_DIR}/source
    ${sdl_ttf_SOURCE_DIR}/include
    ${pigpio_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}/_deps/matplotplusplus-build/source/matplot/
)

target_link_libraries(
    ${PROJECT_NAME}
    PUBLIC
    DSS
    matplot
    SDL3::SDL3
    SDL3_ttf::SDL3_ttf
)

add_subdirectory(${CMAKE_SOURCE_DIR}/src/test)
add_subdirectory(${CMAKE_SOURCE_DIR}/installer)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/robot)