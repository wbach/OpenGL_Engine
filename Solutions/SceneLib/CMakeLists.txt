project(GameEngineCM)
cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(DEFINED ENV{USERROOT})
    message("ENV :")
    message($ENV{USERROOT})
    SET(CMAKE_SYSROOT $ENV{USERROOT})
    include_directories($ENV{USERROOT}/usr/include/)
endif()

IF (WIN32)
  add_compile_options(
    -O3
    -Wa,-mbig-obj
)
ELSE()
  add_compile_options(
    -O3
)
ENDIF()

add_definitions(-DUSE_GNU)

if(DEFINED ENV{USERROOT})
    include_directories($ENV{USERROOT}/usr/include/freetype2)
    include_directories($ENV{USERROOT}/usr/include/minizip)
else()
    include_directories(/usr/include/freetype2)
endif()

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/Utils/
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/UtilsNetwork/
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Tools/common/
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Tools/common/glm
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Tools/common/assimp-5.0.1/include/
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Tools/common/bullet/src/
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Tools/common/rapidxml-1.13/
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/
)

set(SceneDllSources
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/WxEditor/WxEditorSceneDll.cpp
)

SET(EngineCompileFlags "-std=c++20 -Wuninitialized -Wtype-limits -Wdelete-incomplete -Wall -pedantic ${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS} -Wno-expansion-to-defined -Wno-deprecated")


set_source_files_properties(
    ${SceneDllSources}
    PROPERTIES
    COMPILE_FLAGS ${EngineCompileFlags}
)

add_library(WxEditorSceneDll SHARED ${SceneDllSources})
target_include_directories(WxEditorSceneDll PRIVATE ${CMAKE_SOURCE_DIR})
