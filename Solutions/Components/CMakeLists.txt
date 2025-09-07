cmake_minimum_required(VERSION 3.20)
project(GameEngineComponents LANGUAGES CXX)

#cmake -S . -B build -DCOMPONENTS_DIR=/ścieżka/do/Components

# ustawienia kompilatora
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON) # -fPIC

# flaga optymalizacji i warningi
set(COMMON_COMPILE_FLAGS "-O3 -Wall -Wextra -Wuninitialized -Wtype-limits -Wdelete-incomplete -pedantic -Wno-expansion-to-defined -Wno-deprecated -DUSE_GNU")
add_compile_options(${COMMON_COMPILE_FLAGS})

# katalog z komponentami
set(COMPONENTS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Components")

# include paths
include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/Sources"
    "${CMAKE_CURRENT_SOURCE_DIR}/Sources/Utils"
    "${CMAKE_CURRENT_SOURCE_DIR}/Sources/UtilsNetwork"
    "${CMAKE_CURRENT_SOURCE_DIR}/Tools/common"
    "${CMAKE_CURRENT_SOURCE_DIR}/Tools/common/glm"
    "${CMAKE_CURRENT_SOURCE_DIR}/Tools/common/assimp-5.0.1/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/Tools/common/bullet/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/Tools/common/rapidxml-1.13"
    "${CMAKE_CURRENT_SOURCE_DIR}/Tools/common/rapidjson-1.1.0/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/Sources/GameEngine"
    "${CMAKE_CURRENT_SOURCE_DIR}/Tools/common/magic_enum-v0.9.7/include"
    "/usr/include/freetype2"
)

# jeśli istnieje zmienna USERROOT w środowisku
if(DEFINED ENV{USERROOT})
    include_directories(
        "$ENV{USERROOT}/usr/include"
        "$ENV{USERROOT}/usr/include/freetype2"
        "$ENV{USERROOT}/usr/include/minizip"
    )
endif()

# funkcja do dodawania wszystkich komponentów w folderze
file(GLOB COMPONENT_DIRS RELATIVE ${COMPONENTS_DIR} ${COMPONENTS_DIR}/*)

foreach(COMP_NAME ${COMPONENT_DIRS})
    if(IS_DIRECTORY "${COMPONENTS_DIR}/${COMP_NAME}")
        file(GLOB SRC_FILES "${COMPONENTS_DIR}/${COMP_NAME}/*.cpp")
        if(SRC_FILES)
            add_library(${COMP_NAME} SHARED ${SRC_FILES})
            message(STATUS "Dodano komponent: ${COMP_NAME}, źródła: ${SRC_FILES}")
        else()
            message(WARNING "Brak plików .cpp w ${COMP_NAME}, pomijam.")
        endif()
    endif()
endforeach()

