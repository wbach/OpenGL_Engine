project(GameEngineCM)
cmake_minimum_required(VERSION 3.0)

option(BUILD_ALL "Build all" OFF)
option(BUILD_UTILS "Build game engine utils" OFF)
option(BUILD_UTILS_NETWORK "Build network utils" OFF)
option(BUILD_UTILS_NETWORK_TESTS "Build network utils tests" OFF)
option(BUILD_COMMON "Build common" OFF)
option(BUILD_LIB_NOISE "Build libnoise" OFF)
option(BUILD_GAME_ENGINE "Build game engine" OFF)
option(BUILD_TESTS "Build game engine tests" OFF)
option(BUILD_TEST_GAME "Build test game" OFF)
option(BUILD_AVATAR_GAME "Build avatar game" OFF)
option(BUILD_RPG_GAME_SERVER "Build rpg game server" OFF)
option(BUILD_TERRAIN_GENERATOR "Build terrain generator" OFF)
option(BUILD_EDITOR "Build editor" OFF)

option(BUILD_I386 "32bit build" OFF)

if (BUILD_EDITOR)
    SET(BUILD_UTILS ON)
    SET(BUILD_UTILS_NETWORK ON)
    SET(BUILD_UTILS_NETWORK_TESTS ON)
    SET(BUILD_COMMON ON)
    SET(BUILD_GAME_ENGINE ON)
    SET(BUILD_EDITOR ON)
endif()

if(BUILD_ALL)
    SET(BUILD_UTILS ON)
    SET(BUILD_UTILS_NETWORK ON)
    SET(BUILD_UTILS_NETWORK_TESTS ON)
    SET(BUILD_COMMON ON)
    SET(BUILD_LIB_NOISE ON)
    SET(BUILD_GAME_ENGINE ON)
    SET(BUILD_TESTS ON)
    SET(BUILD_TEST_GAME ON)
    SET(BUILD_AVATAR_GAME ON)
    SET(BUILD_RPG_GAME_SERVER ON)
    SET(BUILD_TERRAIN_GENERATOR ON)
    SET(BUILD_EDITOR ON)
endif()

message(STATUS "32bit build: ${BUILD_I386}")
message(STATUS "Build utils: ${BUILD_UTILS}")
message(STATUS "Build network utils: ${BUILD_UTILS_NETWORK}")
message(STATUS "Build network utils tests: ${BUILD_UTILS_NETWORK_TESTS}")
message(STATUS "Build common: ${BUILD_COMMON}")
message(STATUS "Build game engine: ${BUILD_GAME_ENGINE}")
message(STATUS "Build game engine tests: ${BUILD_TESTS}")
message(STATUS "Build test game: ${BUILD_TEST_GAME}")
message(STATUS "Build avatar game: ${BUILD_AVATAR_GAME}")
message(STATUS "Build rpg game server: ${BUILD_RPG_GAME_SERVER}")
message(STATUS "Build terrain generator: ${BUILD_TERRAIN_GENERATOR}")
message(STATUS "Build editor: ${BUILD_EDITOR}")

SET(GCC_COVERAGE_LINK_FLAGS "-static-libgcc")
#CMAKE_CXX_FLAGS
SET(EngineCompileFlags "-std=c++17 -Wuninitialized -Wtype-limits -Wdelete-incomplete -Wall -pedantic ${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS} -Wno-expansion-to-defined -Wno-error=class-memaccess")
SET(CMAKE_CXX_FLAGS "-g -O0")
set(CMAKE_C_FLAGS "-g -O0")
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")

add_compile_options(
    -O3
)

if(BUILD_I386)
    add_compile_options(-m32)
    set_property(GLOBAL PROPERTY COMPILE_FLAGS "-m32")
    set_property(GLOBAL PROPERTY  LINK_FLAGS "-m32")
    #set_target_properties(GLOBAL PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
endif()

include(${CMAKE_CURRENT_SOURCE_DIR}/Sources/WindowsSources.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/Sources/Unix.cmake)

message(STATUS "CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "CMAKE_CURRENT_LIST_DIR: ${CMAKE_CURRENT_LIST_DIR}")
message(STATUS "PROJECT_BINARY_DIR: ${PROJECT_BINARY_DIR}")

message(STATUS ${CMAKE_CURRENT_SOURCE_DIR}/../../Tools/)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/Utils/
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/UtilsNetwork/
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Tools/linux/fbx/
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Tools/common/
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Tools/common/glm
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Tools/common/assimp-5.0.1/include/
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Tools/common/bullet/src/
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Tools/common/rapidxml-1.13/
)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../Tools/linux/fbx/lib/gcc4/x64/release)

set(BulletLinkingLibs
            BulletDynamics
            Bullet3Common
            #Bullet2FileLoader
            Bullet3Collision
            Bullet3Dynamics
            Bullet3Geometry
            Bullet3OpenCL_clew
            BulletCollision
            #BulletFileLoader
            BulletInverseDynamics
            #BulletInverseDynamicsUtils
            #BulletRobotics
            BulletSoftBody
            #BulletWorldImporter
            #BulletXmlWorldImporter
            #ConvexDecomposition
            #GIMPACTUtils
            #HACD
            LinearMath
            fbxsdk
      )

set(BoostLinkingLibs
    boost_system
    boost_filesystem
)

include(${CMAKE_CURRENT_SOURCE_DIR}/Utils.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/Input.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/GraphicsApi.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/OpenGLApi.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/NetworkUtils.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/NetworkUtilsTests.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/Common.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/GameEngine.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/Editor.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/TestGame.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/Avatar.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/GameServer.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/GameEngineTest.cmake)