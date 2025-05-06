cmake_minimum_required(VERSION 3.10)

project(Game)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

FetchContent_Declare(
    freetype
    GIT_REPOSITORY https://gitlab.freedesktop.org/freetype/freetype.git
    GIT_TAG VER-2-13-0
)
FetchContent_MakeAvailable(freetype)

add_subdirectory(external/litehtml)

include_directories(${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/external/litehtml/include)

file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})

find_library(RAYLIB_LIB raylib REQUIRED PATHS /usr/local/lib)

if (NOT RAYLIB_LIB)
    message(FATAL_ERROR "raylib not found in /usr/local/lib")
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE ${RAYLIB_LIB}
    nlohmann_json
    litehtml
    freetype)

if (UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE m dl pthread)
endif()

message(STATUS "Project configured successfully!")