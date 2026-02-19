# This is the build system for Dyver. If you require assistance
# with adding libraries and dependencies, please contact a project manager
# (Most likely Estelle Coonan)

cmake_minimum_required(VERSION 3.16)
project(Dyver VERSION 0.0.0 LANGUAGES CXX)

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
    GIT_TAG master
    GIT_PROGRESS TRUE
    GIT_SHALLOW TRUE
    USES_TERMINAL_DOWNLOAD TRUE
)

FetchContent_MakeAvailable(eigen)
FetchContent_MakeAvailable(dss)
FetchContent_MakeAvailable(sdl)
FetchContent_MakeAvailable(matplotplusplus)

add_subdirectory(src/test)

add_executable(
    ${PROJECT_NAME}
    src/main.cpp
    src/rov.cpp
    src/thruster/thruster.cpp
    src/bindable_action.cpp
    src/app.cpp
)

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
    ${eigen_SOURCE_DIR}
    ${dss_SOURCE_DIR}
    ${sdl_SOURCE_DIR}/include
    ${matplotplusplus_SOURCE_DIR}/source
    ${CMAKE_BINARY_DIR}/_deps/matplotplusplus-build/source/matplot/
    src
)

target_link_libraries(
    ${PROJECT_NAME}
    PUBLIC
    DSS
    matplot
    SDL3::SDL3
)
