cmake_minimum_required(VERSION 3.22)

project(
    ipsum
    VERSION 0.0.0
    LANGUAGES ASM C CXX
)

include(FetchContent)

find_package(Threads)

FetchContent_Declare(
    celero
    GIT_REPOSITORY https://github.com/DigitalInBlue/Celero.git
    GIT_TAG v2.8.5
)

FetchContent_MakeAvailable(
    celero
)

find_package(SDL2 REQUIRED)

add_executable(
    bench
    bench.cpp
)

target_link_libraries(
    bench
    PRIVATE
        celero
)

add_library(
    perfetto
    STATIC
    perfetto.cc
)

add_executable(
    draw
    main.cpp
)

target_include_directories(
    draw
    PRIVATE
        .
        ${SDL2_INCLUDE_DIR}
)

target_link_libraries(
    draw
    PRIVATE
        ${SDL2_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
        perfetto
)

add_executable(
    raycaster
    raycaster.cpp
)

target_link_libraries(
    raycaster
    PRIVATE
        ${CMAKE_THREAD_LIBS_INIT}
        perfetto
)
