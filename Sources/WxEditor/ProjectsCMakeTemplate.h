#pragma once

inline const char* projectCMakeTemplate = R"(cmake_minimum_required(VERSION 3.20)
project(GameEngineComponents LANGUAGES CXX)

# ustawienia kompilatora
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON) # -fPIC

# flaga optymalizacji i warningi
set(COMMON_COMPILE_FLAGS
    -O3
    -Wall
    -Wextra
    -Wuninitialized
    -Wtype-limits
    -Wdelete-incomplete
    -pedantic
    -Wno-expansion-to-defined
    -Wno-deprecated
    -DUSE_GNU
)
add_compile_options(${COMMON_COMPILE_FLAGS})

# katalog komponentow
if(NOT DEFINED COMPONENTS_DIR)
    set(COMPONENTS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Components")
endif()
get_filename_component(ABS_COMPONENTS_DIR "${COMPONENTS_DIR}" ABSOLUTE)
message(STATUS "Katalog komponentow: ${ABS_COMPONENTS_DIR}")

# katalog silnika gry
if(NOT DEFINED ENGINE_INCLUDE_DIR)
    message(FATAL_ERROR "Musisz podac ENGINE_INCLUDE_DIR przy wywolaniu cmake")
endif()
get_filename_component(ABS_ENGINE_INCLUDE_DIR "${ENGINE_INCLUDE_DIR}" ABSOLUTE)
message(STATUS "Include paths silnika gry: ${ABS_ENGINE_INCLUDE_DIR}")

include_directories(
    "${ABS_ENGINE_INCLUDE_DIR}/Sources"
    "${ABS_ENGINE_INCLUDE_DIR}/Sources/Utils"
    "${ABS_ENGINE_INCLUDE_DIR}/Sources/UtilsNetwork"
    "${ABS_ENGINE_INCLUDE_DIR}/Tools/common"
    "${ABS_ENGINE_INCLUDE_DIR}/Tools/common/glm"
    "${ABS_ENGINE_INCLUDE_DIR}/Tools/common/assimp-5.0.1/include"
    "${ABS_ENGINE_INCLUDE_DIR}/Tools/common/bullet/src"
    "${ABS_ENGINE_INCLUDE_DIR}/Tools/common/rapidxml-1.13"
    "${ABS_ENGINE_INCLUDE_DIR}/Tools/common/rapidjson-1.1.0/include"
    "${ABS_ENGINE_INCLUDE_DIR}/Sources/GameEngine"
    "${ABS_ENGINE_INCLUDE_DIR}/Tools/common/magic_enum-v0.9.7/include"
    "/usr/include/freetype2"
)

if(DEFINED ENV{USERROOT})
    include_directories(
        "$ENV{USERROOT}/usr/include"
        "$ENV{USERROOT}/usr/include/freetype2"
        "$ENV{USERROOT}/usr/include/minizip"
    )
endif()

# wykrywanie i budowanie komponentow
file(GLOB COMPONENT_SUBDIRS RELATIVE ${ABS_COMPONENTS_DIR} ${ABS_COMPONENTS_DIR}/*)

foreach(COMP_NAME ${COMPONENT_SUBDIRS})
    set(COMP_PATH "${ABS_COMPONENTS_DIR}/${COMP_NAME}")
    if(IS_DIRECTORY "${COMP_PATH}")
        file(GLOB_RECURSE SRC_FILES "${COMP_PATH}/*.cpp")
        if(SRC_FILES)
            add_library(${COMP_NAME} SHARED ${SRC_FILES})
            message(STATUS "Dodano komponent: ${COMP_NAME}")

            # kopiowanie do folderu nadrzednego
            set(DEST_DIR "${ABS_COMPONENTS_DIR}")
            add_custom_command(TARGET ${COMP_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${COMP_NAME}> ${DEST_DIR}
                COMMENT "Kopiuje ${COMP_NAME} do ${DEST_DIR}"
            )
        else()
            message(WARNING "Brak plikow .cpp w ${COMP_NAME}")
        endif()
    endif()
endforeach()
)";