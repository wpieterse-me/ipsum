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

add_executable(
    bench
    bench.cpp
)

target_link_libraries(
    bench
    PRIVATE
        celero
)
