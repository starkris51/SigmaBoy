cmake_minimum_required(VERSION 3.20)
project(SigmaBoy)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")
add_executable(app ${SOURCES})

find_package(SDL3 REQUIRED)
target_link_libraries(app PRIVATE SDL3::SDL3)
target_include_directories(app PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_compile_definitions(app PRIVATE SDL_MAIN_USE_CALLBACKS)
target_link_options(app PRIVATE -static)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)