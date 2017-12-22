project(GameEngineCM)
cmake_minimum_required(VERSION 3.0)

option(BUILD_UTILS "Build game engine utils" ON)
option(BUILD_UTILS "Build game engine utils" ON)
option(BUILD_LIB_NOISE "Build libnoise" ON)
option(BUILD_GAME_ENGINE "Build game engine" ON)
option(BUILD_TESTS "Build game engine" OFF)
option(BUILD_TEST_GAME "Build test game" OFF)
option(BUILD_TERRAIN_GENERATOR "Build terrain generator" OFF)

message(STATUS "Build game engine utils: ${BUILD_UTILS}")
message(STATUS "Build game engine: ${BUILD_GAME_ENGINE}")
message(STATUS "Build game engine tests: ${BUILD_TESTS}")
message(STATUS "Build test game: ${BUILD_TEST_GAME}")
message(STATUS "Build terrain generator: ${BUILD_TERRAIN_GENERATOR}")

add_compile_options(
    -std=c++14
    -I/usr/include/freetype2
)

include(./Sources/WindowsSources.cmake)
include(./Sources/Unix.cmake)

include_directories(
../Utils/
../Tools/common/
../Tools/common/glm
../Tools/common/rapidxml-1.13/
)

if(BUILD_UTILS)
    include(./Sources/UtilsSources.cmake)
    add_library(UtilsLib ${UtilsSources})
endif()

if(BUILD_GAME_ENGINE OR BUILD_TEST_GAME)
    include(./Sources/GameEngineSources.cmake)
    add_library(GameEngineLib ${GameEngineSources})
endif()

if(BUILD_TEST_GAME)
    include(./Sources/TestGame.cmake)
    add_executable(TestGlGame ${TestGame})
    target_link_libraries(TestGlGame GameEngineLib UtilsLib ${LinkingLibs})
endif()

if(BUILD_LIB_NOISE OR BUILD_TERRAIN_GENERATOR)
    include_directories(
        ../Tools/common/libnoise/
        )
    include(./Sources/LibNoiseSources.cmake)
    add_library(LibNoise ${LibNoiseSources})
endif()

if(BUILD_TERRAIN_GENERATOR)
    
    include(./Sources/TerrainGeneratorSources.cmake)
    add_executable(TerrainGeneratorExe ${TerrainGeneratorSources})
    target_link_libraries(TerrainGeneratorExe LibNoise pthread)
endif()

if(BUILD_TESTS)
    include_directories(
    ../GameEngine/
    )
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../Tools/common/googletest_repo/googletest ${CMAKE_CURRENT_BINARY_DIR}/googletest)
    include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

    include(./Sources/GameEngineTests.cmake)
    add_executable(GAME_ENGINE_TESTS ${GameEngineTests})
    target_link_libraries(GAME_ENGINE_TESTS GameEngineLib gtest UtilsLib SDL2 GL GLU GLEW drm assimp pthread freeimage freetype)
endif()

