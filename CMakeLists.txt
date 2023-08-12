#Cmake version.
cmake_minimum_required(VERSION 3.20.0)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include(ExternalProject)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXE_LINKER_FLAGS "-no-pie")

# Project
project(8080 VERSION 1.0.0 LANGUAGES CXX C DESCRIPTION "8080 emulator")
option(BUILD_DEBUG "Build Debug" ON)
if(BUILD_DEBUG)
    add_definitions(-DDEBUG)
endif(BUILD_DEBUG)

ExternalProject_Add(nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2
    PREFIX ${CMAKE_SOURCE_DIR}/lib/nlohmann_json
    CMAKE_ARGS
    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_SOURCE_DIR}/lib/local)

include_directories(
    ${CMAKE_SOURCE_DIR}/lib/local/include)

link_directories(
    ${CMAKE_SOURCE_DIR}/lib/local/lib/
    ${CMAKE_SOURCE_DIR}/lib/local/lib64/)

file(GLOB SRC
    src/*.cpp)

add_executable(${CMAKE_PROJECT_NAME} ${SRC})
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC) 
add_custom_target(run ${CMAKE_PROJECT_NAME} data/gb-test-roms-master/cpu_instrs/cpu_instrs.gb
    DEPENDS ${SAMPLE}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
