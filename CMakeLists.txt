cmake_minimum_required(VERSION 3.31)
project(Project_1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp ")
find_package(OpenMP REQUIRED)

configure_file(${CMAKE_SOURCE_DIR}/Resources/dati.csv
        ${CMAKE_BINARY_DIR}/Resources/dati.csv COPYONLY)

include_directories(include)

set(SOURCE_FILES src/main.cpp)
add_executable(Project_1 src/main.cpp
        Include/utils.h
        Include/utils.cpp)
target_link_libraries(Project_1 OpenMP::OpenMP_CXX)
