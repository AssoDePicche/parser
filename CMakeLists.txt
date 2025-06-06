cmake_minimum_required(VERSION 3.10)

project(Parser C)

set(CMAKE_C_STANDARD 23)

set(CMAKE_C_STANDARD_REQUIRED True)

include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/source/*.c")

add_executable(${PROJECT_NAME} ${SOURCES})

set(COMPILE_OPTIONS -Wall -Wextra -Werror -Wpedantic -g)

target_compile_options(${PROJECT_NAME} PRIVATE ${COMPILE_OPTIONS})
