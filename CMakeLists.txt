cmake_minimum_required(VERSION 3.22)

project(
    ipsum
    VERSION 0.0.0
    LANGUAGES ASM C CXX
)

include(FetchContent)

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

add_executable(
    draw
    main.cpp
)

target_include_directories(
    draw
    PRIVATE
        ${SDL2_INCLUDE_DIR}
)

target_link_libraries(
    draw
    ${SDL2_LIBRARIES}
)
