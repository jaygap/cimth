cmake_minimum_required(VERSION 3.20)
project(cimth)

include_directories(${CMAKE_SOURCE_DIR}/inc)
link_directories(${CMAKE_SOURCE_DIR}/lib)

add_link_options(-lm)

add_executable(cimth src/main.c)
