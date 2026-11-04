
include(ExternalProject)

set(SDL2_TTF_SOURCE_DIR
    "${CMAKE_SOURCE_DIR}/../../Tools/common/SDL2_ttf"
)

set(SDL2_TTF_BUILD_DIR
    "${CMAKE_BINARY_DIR}/SDL2_ttf-build"
)

set(SDL2_TTF_INSTALL_DIR
    "${CMAKE_BINARY_DIR}/SDL2_ttf-install"
)

set(SDL2_TTF_LIBRARY
    "${SDL2_TTF_INSTALL_DIR}/lib/libSDL2_ttf.a"
)

set(FREETYPE_LIBRARY
    "${SDL2_TTF_BUILD_DIR}/external/freetype/libfreetype.a"
)

# Katalog istnieje już podczas konfiguracji CMake.
file(MAKE_DIRECTORY
    "${SDL2_TTF_INSTALL_DIR}/include"
)

ExternalProject_Add(SDL2_ttfExternal
    SOURCE_DIR "${SDL2_TTF_SOURCE_DIR}"
    BINARY_DIR "${SDL2_TTF_BUILD_DIR}"

    CMAKE_ARGS
        -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
        -DCMAKE_BUILD_TYPE=Release
        -DCMAKE_INSTALL_PREFIX=${SDL2_TTF_INSTALL_DIR}
        -DCMAKE_INSTALL_LIBDIR=lib
        -DBUILD_SHARED_LIBS=OFF
        -DSDL2_LIBRARY:FILEPATH=${SDL2_LIBRARY}
        -DSDL2_INCLUDE_DIR:PATH=${SDL2_INSTALL_DIR}/include/SDL2
        -DSDL2TTF_VENDORED=ON
        -DSDL2TTF_SAMPLES=OFF
        -DSDL2TTF_HARFBUZZ=OFF
        -DCMAKE_C_FLAGS=-pthread
        -DCMAKE_CXX_FLAGS=-pthread

    BUILD_COMMAND
        ${CMAKE_COMMAND} --build <BINARY_DIR> --config Release

    INSTALL_COMMAND
        ${CMAKE_COMMAND} --install <BINARY_DIR> --config Release

    BUILD_IN_SOURCE FALSE
)

# SDL2 musi zostać zbudowane i zainstalowane przed SDL2_ttf.
add_dependencies(SDL2_ttfExternal SDL2External)

add_library(SDL2_ttfLib STATIC IMPORTED GLOBAL)

set_target_properties(SDL2_ttfLib PROPERTIES
    IMPORTED_LOCATION "${SDL2_TTF_LIBRARY}"

    INTERFACE_INCLUDE_DIRECTORIES
        "${SDL2_TTF_INSTALL_DIR}/include"

    INTERFACE_LINK_LIBRARIES
        "SDL2Lib;${FREETYPE_LIBRARY}"
)

add_dependencies(SDL2_ttfLib SDL2_ttfExternal)
