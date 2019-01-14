project(GameEngineCM)
cmake_minimum_required(VERSION 3.0)

option(BUILD_ALL "Build all" OFF)
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

if(BUILD_ALL)
    SET(BUILD_UTILS ON)
    SET(BUILD_UTILS_NETWORK ON)
    SET(BUILD_COMMON ON)
   # SET(BUILD_LIB_NOISE ON)
    SET(BUILD_GAME_ENGINE ON)
    SET(BUILD_TESTS ON)
    SET(BUILD_TEST_GAME ON)
    SET(BUILD_RPG_GAME_SERVER ON)
   # SET(BUILD_TERRAIN_GENERATOR ON)
    SET(BUILD_I386 ON)
endif()

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
SET( CMAKE_CXX_FLAGS  "-std=c++14 -Wuninitialized -Wtype-limits -Wdelete-incomplete -Wall -pedantic ${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS} -Wno-expansion-to-defined" )
SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}" )

add_compile_options(
    -std=c++17
)

if(BUILD_I386)
    add_compile_options(-m32)
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
    ../../Tools/common/bullet/src/
    ../../Tools/common/rapidxml-1.13/
)

if(BUILD_UTILS)
    include(./Sources/UtilsSources.cmake)
    add_library(UtilsLib ${UtilsSources})
    if(BUILD_I386)
        set_target_properties(UtilsLib PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
    endif()
endif()

if(BUILD_UTILS_NETWORK)
    include(./Sources/UtilsNetworkSources.cmake)
    add_library(UtilsNetworkLib ${UtilsNetworkSources})
    if(BUILD_I386)
        set_target_properties(UtilsNetworkLib PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
    endif()
endif()

if(BUILD_COMMON)
    include(./Sources/CommonSources.cmake)
    add_library(CommonLib ${CommonSources})
    if(BUILD_I386)
        set_target_properties(CommonLib PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
    endif()
endif()

if(BUILD_GAME_ENGINE OR BUILD_TEST_GAME)
    include(./Sources/GameEngineSources.cmake)
    add_library(GameEngineLib ${GameEngineSources})
    if(BUILD_I386)
        set_target_properties(GameEngineLib PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
    endif()
endif()


#DBUILD_PYBULLET=ON -DBUILD_PYBULLET_NUMPY=ON -DUSE_DOUBLE_PRECISION=ON -DBT_USE_EGL=ON -DCMAKE_BUILD_TYPE=Release
#option(BUILD_PYBULLET "" ON)
#option(BUILD_PYBULLET_NUMPY "" ON)
#option(USE_DOUBLE_PRECISION "" ON)
#option(BT_USE_EGL "" OFF)
option(BUILD_UNIT_TESTS "" OFF)
// option(BUILD_EXTRAS "" OFF)
// option(BUILD_BULLET2_DEMOS "" OFF)
// option(BUILD_OPENGL3_DEMOS "" OFF)



 set(BulletLinkingLibs
            Bullet3Common
            Bullet2FileLoader
            Bullet3Collision
            Bullet3Dynamics
            Bullet3Geometry
            Bullet3OpenCL_clew
            BulletCollision
            BulletDynamics
            BulletFileLoader
            BulletInverseDynamics
            BulletInverseDynamicsUtils
            BulletRobotics
            BulletSoftBody
            BulletWorldImporter
            BulletXmlWorldImporter
            BussIK
            clsocket
            ConvexDecomposition
            GIMPACTUtils
            HACD
            OpenGLWindow
            BulletExampleBrowserLib
            gwen
            LinearMath
      )
link_directories(${CMAKE_CURRENT_BINARY_DIR}/bullet)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../Tools/common/bullet/ ${CMAKE_CURRENT_BINARY_DIR}/bullet)

if(BUILD_TEST_GAME)
    include_directories(
    ../../Sources/GameEngine/
    )
    include(./Sources/TestGameSources.cmake)
    add_executable(TestGlGame ${TestGameSources})
    if(BUILD_I386)
        set_target_properties(TestGlGame PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
    endif()
    target_link_libraries(TestGlGame GameEngineLib UtilsNetworkLib CommonLib UtilsLib ${LinkingLibs} ${BulletLinkingLibs})
endif()

if(BUILD_RPG_GAME_SERVER)
    include(./Sources/RpgGameServerSources.cmake)
    add_executable(RpgGameServer ${RpgGameServerSources})
    if(BUILD_I386)
        set_target_properties(RpgGameServer PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
    endif()
    target_link_libraries(RpgGameServer  UtilsNetworkLib CommonLib UtilsLib ${LinkingLibs})
endif()

if(BUILD_LIB_NOISE OR BUILD_TERRAIN_GENERATOR)
    include_directories(
        ../../Tools/common/libnoise/
        ../../Tools/common/libnoise/src/
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
    
    include(./Sources/GameEngineTestsSources.cmake)
    add_executable(GAME_ENGINE_TESTS ${GameEngineTestsSources})
    if(BUILD_I386)
        set_target_properties(GAME_ENGINE_TESTS PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
    endif()
    target_link_libraries(GAME_ENGINE_TESTS GameEngineLib UtilsNetworkLib CommonLib UtilsLib gtest gtest_main gmock gmock_main ${LinkingLibs})
endif()

