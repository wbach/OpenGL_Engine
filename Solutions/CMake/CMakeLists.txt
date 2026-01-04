cmake_minimum_required(VERSION 4.0)
project(GameEngineCM LANGUAGES CXX)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    add_compile_definitions(_GLIBCXX_USE_CXX11_ABI=1)
endif()

set(CMAKE_CONFIGURATION_TYPES
    "Debug;Release;RelWithDebInfo;MinSizeRel"
    CACHE STRING "Available build configurations" FORCE)

# --- Kompilacja ---
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(DEFINED ENV{DISTCC})
    set(CMAKE_C_COMPILER_LAUNCHER   "/usr/bin/distcc"  CACHE STRING "C compiler" FORCE)
    set(CMAKE_CXX_COMPILER_LAUNCHER "/usr/bin/distcc" CACHE STRING "C++ compiler" FORCE)
endif()

message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    #add_compile_options(-Wall -Wextra -Wpedantic)

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        message(STATUS "Debug build (GCC/Clang) with sanitizers")
        add_compile_options(-O0 -g -fsanitize=address,undefined -fno-omit-frame-pointer)
        add_link_options(-fsanitize=address,undefined -fno-omit-frame-pointer)

    elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
        message(STATUS "Release build (GCC/Clang)")
        add_compile_options(-O3 -DNDEBUG)

    elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        message(STATUS "RelWithDebInfo build (GCC/Clang)")
        add_compile_options(-O2 -g -DNDEBUG)

    elseif(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
        message(STATUS "MinSizeRel build (GCC/Clang)")
        add_compile_options(-Os -DNDEBUG)
    endif()

elseif(MSVC)
    add_compile_options(/W4 /permissive-)

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        message(STATUS "Debug build (MSVC)")
        add_compile_options(/Zi /Od)

    elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
        message(STATUS "Release build (MSVC)")
        add_compile_options(/O2 /DNDEBUG)

    elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        message(STATUS "RelWithDebInfo build (MSVC)")
        add_compile_options(/O2 /Zi /DNDEBUG)

    elseif(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
        message(STATUS "MinSizeRel build (MSVC)")
        add_compile_options(/O1 /DNDEBUG)
    endif()
endif()


# --- Domyślne flagi i linker ---
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pipe")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pipe")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -rdynamic")

set(ENGINE_COMPILE_FLAGS
    -std=c++20
    -Wall
    -pedantic
    -Wuninitialized
    -Wtype-limits
    -Wdelete-incomplete
    -Wno-expansion-to-defined
    -Wno-deprecated
    -Wno-class-memaccess

)

if(WIN32)
    add_compile_options(-O3 -Wa,-mbig-obj)
endif()

# --- Środowisko (USERROOT) ---
if(DEFINED ENV{USERROOT})
    message(STATUS "Using sysroot: $ENV{USERROOT}")
    set(CMAKE_SYSROOT $ENV{USERROOT})
    include_directories($ENV{USERROOT}/usr/include)
endif()

# --- Opcje budowania ---
option(BUILD_ALL                 "Build everything" OFF)
option(BUILD_UTILS               "Build game engine utils" OFF)
option(BUILD_UTILS_NETWORK       "Build network utils" OFF)
option(BUILD_UTILS_NETWORK_TESTS "Build network utils tests" OFF)
option(BUILD_COMMON              "Build common" OFF)
option(BUILD_GAME_ENGINE         "Build game engine" ON)
option(BUILD_TESTS               "Build tests" OFF)
option(BUILD_BENCHMARK           "Build benchmark" OFF)
option(BUILD_GAME                "Build avatar game" ON)
option(BUILD_RPG_GAME_SERVER     "Build RPG game server" OFF)
option(BUILD_WX_EDITOR           "Build wxEditor" OFF)
option(BUILD_I386                "32bit build" OFF)


message(STATUS "BUILD_UTILS_NETWORK_TESTS: ${BUILD_UTILS_NETWORK_TESTS}")
message(STATUS "BUILD_WX_EDITOR: ${BUILD_WX_EDITOR}")
message(STATUS "BUILD_GAME: ${BUILD_GAME}")
message(STATUS "BUILD_TESTS: ${BUILD_TESTS}")
message(STATUS "BUILD_GAME_ENGINE: ${BUILD_GAME_ENGINE}")

# --- Makro do włączania komponentów ---
macro(enable_components)
    foreach(comp ${ARGV})
        set(${comp} ON CACHE BOOL "" FORCE)
    endforeach()
endmacro()

if(BUILD_ALL)
    enable_components(
        BUILD_UTILS
        BUILD_UTILS_NETWORK
        BUILD_UTILS_NETWORK_TESTS
        BUILD_COMMON
        BUILD_GAME_ENGINE
        BUILD_TESTS
        BUILD_GAME
        BUILD_RPG_GAME_SERVER
        BUILD_WX_EDITOR
    )
endif()

if(BUILD_WX_EDITOR)
    enable_components(BUILD_UTILS BUILD_UTILS_NETWORK BUILD_COMMON BUILD_GAME_ENGINE)
    set(BUILD_EDITOR OFF CACHE BOOL "" FORCE)
endif()

if(BUILD_GAME)
    enable_components(BUILD_UTILS BUILD_UTILS_NETWORK BUILD_COMMON BUILD_GAME_ENGINE)
endif()

if(BUILD_TESTS)
    enable_components(BUILD_UTILS BUILD_UTILS_NETWORK BUILD_UTILS_NETWORK_TESTS BUILD_COMMON BUILD_GAME_ENGINE)
endif()

# --- 32-bit ---
if(BUILD_I386)
    add_compile_options(-m32)
    set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -m32")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -m32")
endif()

# --- Ścieżki globalne ---
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/Utils/
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/UtilsNetwork/
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Tools/common/
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Tools/common/glm
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Tools/common/assimp-5.0.1/include/
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Tools/common/bullet/src/
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Tools/common/rapidxml-1.13/
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Tools/common/rapidjson-1.1.0/include/
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Tools/common/magic_enum-v0.9.7/include/
)

# --- Biblioteki Bullet ---
set(BULLET_LIBS
    BulletDynamics
    Bullet3Common
    Bullet3Collision
    Bullet3Dynamics
    Bullet3Geometry
    Bullet3OpenCL_clew
    BulletCollision
    BulletInverseDynamics
    BulletSoftBody
    LinearMath
)

# --- Includy plików cmake ---
include(${CMAKE_CURRENT_SOURCE_DIR}/Sources/WindowsSources.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/Sources/Unix.cmake)

include(${CMAKE_CURRENT_SOURCE_DIR}/Utils.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/Input.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/GraphicsApi.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/OpenGLApi.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/NetworkUtils.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/NetworkUtilsTests.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/Common.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/GameEngine.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/WxEditor.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/Game.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/GameServer.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/Tests.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/Benchmark.cmake)

# --- Debug info ---
message(STATUS "Build configuration:")
foreach(opt
    BUILD_I386 BUILD_UTILS BUILD_UTILS_NETWORK BUILD_UTILS_NETWORK_TESTS
    BUILD_COMMON BUILD_GAME_ENGINE BUILD_TESTS BUILD_GAME
    BUILD_RPG_GAME_SERVER BUILD_WX_EDITOR
)
    message(STATUS "  ${opt}: ${${opt}}")
endforeach()

message(STATUS "CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "CMAKE_CURRENT_LIST_DIR:  ${CMAKE_CURRENT_LIST_DIR}")
message(STATUS "PROJECT_BINARY_DIR:      ${PROJECT_BINARY_DIR}")
