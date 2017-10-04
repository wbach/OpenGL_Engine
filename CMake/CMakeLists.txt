project(GameEngineCM)
cmake_minimum_required(VERSION 3.0)

option(BUILD_GAME_ENGINE "Build game engine" ON)
option(BUILD_TESTS "Build game engine" OFF)
option(BUILD_TEST_GAME "Build test game" OFF)

message(STATUS "Build game engine: ${BUILD_GAME_ENGINE}")
message(STATUS "Build game engine tests: ${BUILD_TESTS}")
message(STATUS "Build test game: ${BUILD_TEST_GAME}")

add_compile_options(
    -std=c++14
    -fdiagnostics-color=auto
    -g
    -ffunction-sections
    -fdata-sections
    -Wno-missing-field-initializers
    -fPIC
    -DDDAL_DISABLE_DEPRECATED_WARNING
    -I/usr/include/freetype2
)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(GL REQUIRED gl)
pkg_check_modules(GLEW REQUIRED glew)
pkg_check_modules(ASSIMP REQUIRED assimp)
#pkg_check_modules(FREEIMAGE REQUIRED freeimage)
pkg_check_modules(FREETYPE REQUIRED freetype2)
pkg_check_modules(GLU REQUIRED glu)

include_directories(
../Utils/
../Tools/common/
../Tools/common/glm
../Tools/common/rapidxml-1.13/
)

include(UtilsSources.cmake)
include(GameEngineSources.cmake)
include(TestGame.cmake)

add_library(UtilsLib ${UtilsSources})
add_library(GameEngineLib ${GameEngineSources})

if(BUILD_TEST_GAME)
    add_executable(TestGlGame ${TestGame})
    target_link_libraries(TestGlGame GameEngineLib UtilsLib SDL2 GL GLU GLEW drm assimp pthread freeimage freetype)
endif()

if(BUILD_TESTS)
    include_directories(
    ../GameEngine/
    )
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../Tools/common/googletest_repo/googletest ${CMAKE_CURRENT_BINARY_DIR}/googletest)
    include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

    include(GameEngineTests.cmake)
    add_executable(GAME_ENGINE_TESTS ${GameEngineTests})
    target_link_libraries(GAME_ENGINE_TESTS GameEngineLib gtest UtilsLib SDL2 GL GLU GLEW drm assimp pthread freeimage freetype)
endif()

