project(GameEngineCM)
cmake_minimum_required(VERSION 3.0)

option(BUILD_UTILS "Build game engine utils" ON)
option(BUILD_UTILS_NETWORK "Build game engine network utils" ON)
option(BUILD_COMMON "Build common" ON)
option(BUILD_LIB_NOISE "Build libnoise" OFF)
option(BUILD_GAME_ENGINE "Build game engine" ON)
option(BUILD_TESTS "Build game engine tests" OFF)
option(BUILD_TEST_GAME "Build test game" OFF)
option(BUILD_RPG_GAME_SERVER "Build rpg game server" OFF)
option(BUILD_TERRAIN_GENERATOR "Build terrain generator" OFF)
option(BUILD_I386 "32bit build" ON)

message(STATUS "32bit build: ${BUILD_I386}")
message(STATUS "Build game engine utils: ${BUILD_UTILS}")
message(STATUS "Build game engine network utils: ${BUILD_UTILS_NETWORK}")
message(STATUS "Build common: ${BUILD_COMMON}")
message(STATUS "Build game engine: ${BUILD_GAME_ENGINE}")
message(STATUS "Build game engine tests: ${BUILD_TESTS}")
message(STATUS "Build test game: ${BUILD_TEST_GAME}")
message(STATUS "Build rpg game server: ${BUILD_RPG_GAME_SERVER}")
message(STATUS "Build terrain generator: ${BUILD_TERRAIN_GENERATOR}")

SET(GCC_COVERAGE_LINK_FLAGS    "-static-libgcc")
SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )
SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}" )

add_compile_options(
    -std=c++14
)

if(BUILD_I386)
	add_compile_options( -m32	)
        set_property(GLOBAL PROPERTY COMPILE_FLAGS "-m32")
	 set_property(GLOBAL PROPERTY  LINK_FLAGS "-m32")
	#set_target_properties(GLOBAL PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
endif()

include(./Sources/WindowsSources.cmake)
include(./Sources/Unix.cmake)

include_directories(
../../Sources/
../../Sources/Utils/
../../Sources/UtilsNetwork/
../../Tools/common/
../../Tools/common/glm
../../Tools/common/rapidxml-1.13/
)

if(BUILD_UTILS)
    include(./Sources/UtilsSources.cmake)
    add_library(UtilsLib ${UtilsSources})
    set_target_properties(UtilsLib PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
endif()

if(BUILD_UTILS_NETWORK)
    include(./Sources/UtilsNetworkSources.cmake)
    add_library(UtilsNetworkLib ${UtilsNetworkSources})
set_target_properties(UtilsNetworkLib PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
endif()

if(BUILD_COMMON)
    include(./Sources/CommonSources.cmake)
    add_library(CommonLib ${CommonSources})
    set_target_properties(CommonLib PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
endif()

if(BUILD_GAME_ENGINE OR BUILD_TEST_GAME)
    include(./Sources/GameEngineSources.cmake)
    add_library(GameEngineLib ${GameEngineSources})
set_target_properties(GameEngineLib PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
endif()

if(BUILD_TEST_GAME)
	include_directories(
	../../Sources/GameEngine/
	)
    include(./Sources/TestGameSources.cmake)
    add_executable(TestGlGame ${TestGameSources})
    set_target_properties(TestGlGame PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
    target_link_libraries(TestGlGame GameEngineLib UtilsNetworkLib CommonLib UtilsLib ${LinkingLibs})
endif()

if(BUILD_RPG_GAME_SERVER)
    include(./Sources/RpgGameServerSources.cmake)
    add_executable(RpgGameServer ${RpgGameServerSources})
    set_target_properties(RpgGameServer PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
    target_link_libraries(RpgGameServer  UtilsNetworkLib CommonLib UtilsLib ${LinkingLibs})
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
	../../Sources/GameEngine/
	)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../Tools/common/googletest ${CMAKE_CURRENT_BINARY_DIR}/googletest)
    include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
	
    include(./Sources/GameEngineTests.cmake)
    add_executable(GAME_ENGINE_TESTS ${GameEngineTests})
    set_target_properties(GAME_ENGINE_TESTS PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
    target_link_libraries(GAME_ENGINE_TESTS GameEngineLib UtilsNetworkLib CommonLib UtilsLib ${LinkingLibs} gtest gtest_main gmock gmock_main)
endif()

