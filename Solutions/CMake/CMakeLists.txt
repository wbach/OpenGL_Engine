cmake_minimum_required(VERSION 3.14)
project(GameEngineCM)

if(DEFINED ENV{USERROOT})
    message("ENV :")
    message($ENV{USERROOT})
    SET(CMAKE_SYSROOT $ENV{USERROOT})
    include_directories($ENV{USERROOT}/usr/include/)
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()


include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

  # SOURCE_DIR     ${UNITY_OUT_DIR}/src
  # BINARY_DIR     ${UNITY_OUT_DIR}/bin
#set(BINARY_DIR CMAKE_CURRENT_BINARY_DIR)
set(ALL_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/libsOut)

FetchContent_Declare(
  bullet
  GIT_REPOSITORY https://github.com/bulletphysics/bullet3.git
  GIT_TAG 3.25
#  BINARY_DIR ${ALL_BINARY_DIR}/bin
)

FetchContent_Declare(
  assimp
  GIT_REPOSITORY https://github.com/assimp/assimp.git
  GIT_TAG v5.3.1
)

FetchContent_Declare(
  sdl
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG release-2.30.1
)

FetchContent_Declare(
  sdlttf
  GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
  GIT_TAG release-2.22.0
)

FetchContent_Declare(
  sdlnet
  GIT_REPOSITORY https://github.com/libsdl-org/SDL_net.git
  GIT_TAG release-2.2.0
)

FetchContent_Declare(
  sdlimage
  GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
  GIT_TAG release-2.8.2
)
FetchContent_MakeAvailable(bullet assimp sdl sdlttf sdlnet sdlimage)

FetchContent_GetProperties(bullet)
if(NOT bullet_POPULATED)
  FetchContent_Populate(bullet)
  add_subdirectory(${bullet_SOURCE_DIR} ${bullet_BINARY_DIR})
endif()
MESSAGE("XXXXXXXXXX")
MESSAGE(${bullet_BINARY_DIR})
MESSAGE(${bullet_SOURCE_DIR})

include_directories(${bullet_SOURCE_DIR}/src/)

enable_testing()

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-error")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BUILD_CPU_DEMOS OFF)

add_definitions(-DUSE_GNU)

include_directories(
     ${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/
     ${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/Utils/
     ${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/UtilsNetwork/
    #${CMAKE_CURRENT_SOURCE_DIR}/../../Tools/linux/fbx/
    #${CMAKE_CURRENT_SOURCE_DIR}/../../Tools/common/
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Tools/common/glm
    # ${CMAKE_CURRENT_SOURCE_DIR}/../../Tools/common/assimp-5.0.1/include/
    # ${CMAKE_CURRENT_SOURCE_DIR}/../../Tools/common/bullet/src/
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Tools/common/rapidxml-1.13/
)

include(${CMAKE_CURRENT_SOURCE_DIR}/Utils.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/NetworkUtils.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/Common.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/Input.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/GraphicsApi.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/OpenGLApi.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/GameEngine.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/Avatar.cmake)
#target_link_libraries(cpp_test PRIVATE sum_integers Catch2::Catch2)


# include(ExternalProject)
# ExternalProject_Add(bullet
#   GIT_REPOSITORY https://github.com/bulletphysics/bullet3.git
#   EXCLUDE_FROM_ALL TRUE
#   BUILD_COMMAND $(MAKE) bullet
#   STEP_TARGETS build)

# set(bullet_BINARY_DIR "${CMAKE_BINARY_DIR}/bullet-prefix/src/bullet-build")
# set(bullet_SOURCE_DIR "${CMAKE_BINARY_DIR}/bullet-prefix/src/bullet/include")

# add_executable(
#   hello_test
#   hello_test.cc
# )
# target_link_libraries(
#   hello_test
#   GTest::gtest_main
# )

# include(GoogleTest)
# gtest_discover_tests(hello_test)