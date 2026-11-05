
include(ExternalProject)

set(SDL2_SOURCE_DIR
    "${CMAKE_SOURCE_DIR}/../../Tools/common/SDL2"
)

set(SDL2_BUILD_DIR
    "${CMAKE_BINARY_DIR}/SDL2-build"
)

set(SDL2_INSTALL_DIR
    "${CMAKE_BINARY_DIR}/SDL2-install"
)

set(SDL2_LIBRARY
    "${SDL2_INSTALL_DIR}/lib/libSDL2.a"
)

ExternalProject_Add(SDL2External
    SOURCE_DIR "${SDL2_SOURCE_DIR}"
    BINARY_DIR "${SDL2_BUILD_DIR}"

    CMAKE_ARGS
        "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
        "-DCMAKE_BUILD_TYPE=Release"
        "-DCMAKE_INSTALL_PREFIX=${SDL2_INSTALL_DIR}"
        "-DCMAKE_INSTALL_LIBDIR=lib"
        "-DBUILD_SHARED_LIBS=OFF"
        "-DSDL_SHARED=OFF"
        "-DSDL_STATIC=ON"
        "-DSDL_TEST=OFF"
        "-DSDL_TESTS=OFF"
        "-DSDL_EXAMPLES=OFF"
        "-DCMAKE_C_FLAGS=-pthread -sMEMORY64=1"
        "-DCMAKE_CXX_FLAGS=-pthread -sMEMORY64=1"

    BUILD_COMMAND
        ${CMAKE_COMMAND} --build <BINARY_DIR> --config Release

    INSTALL_COMMAND
        ${CMAKE_COMMAND} --install <BINARY_DIR> --config Release

    BUILD_IN_SOURCE FALSE
)

add_library(SDL2Lib STATIC IMPORTED GLOBAL)

set_target_properties(SDL2Lib PROPERTIES
    IMPORTED_LOCATION "${SDL2_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${SDL2_SOURCE_DIR}/include"
)

add_dependencies(SDL2Lib SDL2External)
