cmake_minimum_required(VERSION 3.10)

project(SuzieQ C)

set(CMAKE_C_STANDARD 23)

set(CMAKE_C_STANDARD_REQUIRED True)

# Add include directory
include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/source/*.c")

add_executable(${PROJECT_NAME} ${SOURCES})
