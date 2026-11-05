
include(ExternalProject)

# ------------------------------------------------------------
# Paths
# ------------------------------------------------------------

get_filename_component(
    FREEIMAGE_SOURCE_DIR
    "${CMAKE_CURRENT_LIST_DIR}/../../../Tools/common/FreeImage3180"
    ABSOLUTE
)

set(FREEIMAGE_BUILD_SOURCE_DIR
    "${CMAKE_BINARY_DIR}/_deps/FreeImage-src"
)

set(FREEIMAGE_EXTERNAL_PREFIX
    "${CMAKE_BINARY_DIR}/FreeImageExternal-prefix"
)

set(FREEIMAGE_LIBRARY
    "${FREEIMAGE_BUILD_SOURCE_DIR}/libfreeimage.a"
)

set(FREEIMAGE_HEADER_DIR
    "${FREEIMAGE_SOURCE_DIR}/Source"
)

# ------------------------------------------------------------
# Validate
# ------------------------------------------------------------

if(NOT EXISTS "${FREEIMAGE_SOURCE_DIR}/Makefile.gnu")
    message(FATAL_ERROR
        "FreeImage Makefile.gnu not found: ${FREEIMAGE_SOURCE_DIR}"
    )
endif()

if(NOT EXISTS "${FREEIMAGE_SOURCE_DIR}/Makefile.srcs")
    message(FATAL_ERROR
        "FreeImage Makefile.srcs not found: ${FREEIMAGE_SOURCE_DIR}"
    )
endif()

if(NOT EMSCRIPTEN)
    message(FATAL_ERROR
        "FreeImage WebAssembly build requires Emscripten"
    )
endif()

message(STATUS "FreeImage source: ${FREEIMAGE_SOURCE_DIR}")
message(STATUS "FreeImage build:  ${FREEIMAGE_BUILD_SOURCE_DIR}")
message(STATUS "FreeImage target: WASM64")

# ------------------------------------------------------------
# Compiler flags
# ------------------------------------------------------------

set(FREEIMAGE_CFLAGS
    "-O2 -pthread -sMEMORY64=1 -fexceptions -fvisibility=hidden -DOPJ_STATIC -DNO_LCMS -DDISABLE_PERF_MEASUREMENT -D__ANSI__ -I. -ISource -ISource/Metadata -ISource/FreeImageToolkit -ISource/LibJPEG -ISource/LibPNG -ISource/LibTIFF4 -ISource/ZLib -ISource/LibOpenJPEG -ISource/OpenEXR -ISource/OpenEXR/Half -ISource/OpenEXR/Iex -ISource/OpenEXR/IlmImf -ISource/OpenEXR/IlmThread -ISource/OpenEXR/Imath -ISource/OpenEXR/IexMath -ISource/LibRawLite -ISource/LibRawLite/dcraw -ISource/LibRawLite/internal -ISource/LibRawLite/libraw -ISource/LibRawLite/src -ISource/LibWebP -ISource/LibJXR -ISource/LibJXR/common/include -ISource/LibJXR/image/sys -ISource/LibJXR/jxrgluelib"
)

set(FREEIMAGE_CXXFLAGS
    "-O2 -pthread -sMEMORY64=1 -fexceptions -std=c++14 -fvisibility=hidden -D__ANSI__ -I. -ISource -ISource/Metadata -ISource/FreeImageToolkit -ISource/LibJPEG -ISource/LibPNG -ISource/LibTIFF4 -ISource/ZLib -ISource/LibOpenJPEG -ISource/OpenEXR -ISource/OpenEXR/Half -ISource/OpenEXR/Iex -ISource/OpenEXR/IlmImf -ISource/OpenEXR/IlmThread -ISource/OpenEXR/Imath -ISource/OpenEXR/IexMath -ISource/LibRawLite -ISource/LibRawLite/dcraw -ISource/LibRawLite/internal -ISource/LibRawLite/libraw -ISource/LibRawLite/src -ISource/LibWebP -ISource/LibJXR -ISource/LibJXR/common/include -ISource/LibJXR/image/sys -ISource/LibJXR/jxrgluelib"
)

# ------------------------------------------------------------
# Prepare sources script
# ------------------------------------------------------------

set(FREEIMAGE_PREPARE_SCRIPT
    "${CMAKE_BINARY_DIR}/CMakeFiles/PrepareFreeImage.cmake"
)

file(WRITE "${FREEIMAGE_PREPARE_SCRIPT}" [=[
if(NOT DEFINED SOURCE_DIR OR NOT DEFINED DEST_DIR)
    message(FATAL_ERROR "SOURCE_DIR and DEST_DIR are required")
endif()

if(NOT EXISTS "${SOURCE_DIR}/Makefile.gnu")
    message(FATAL_ERROR "FreeImage sources not found: ${SOURCE_DIR}")
endif()

message(STATUS "Preparing FreeImage build sources")

file(REMOVE_RECURSE "${DEST_DIR}")
file(MAKE_DIRECTORY "${DEST_DIR}")

file(COPY "${SOURCE_DIR}/"
    DESTINATION "${DEST_DIR}"

    PATTERN ".git" EXCLUDE
    PATTERN "*.o" EXCLUDE
    PATTERN "*.obj" EXCLUDE
    PATTERN "*.a" EXCLUDE
    PATTERN "*.so" EXCLUDE
    PATTERN "*.so.*" EXCLUDE
    PATTERN "*.dll" EXCLUDE
    PATTERN "*.lib" EXCLUDE
    PATTERN "*.d" EXCLUDE
    PATTERN "CMakeFiles" EXCLUDE
    PATTERN "build" EXCLUDE
    PATTERN "build_*" EXCLUDE
)

message(STATUS "FreeImage sources prepared")
]=])

# ------------------------------------------------------------
# External project
# ------------------------------------------------------------

ExternalProject_Add(FreeImageExternal

    PREFIX "${FREEIMAGE_EXTERNAL_PREFIX}"

    SOURCE_DIR "${FREEIMAGE_BUILD_SOURCE_DIR}"

    DOWNLOAD_COMMAND
        ${CMAKE_COMMAND}
        "-DSOURCE_DIR=${FREEIMAGE_SOURCE_DIR}"
        "-DDEST_DIR=${FREEIMAGE_BUILD_SOURCE_DIR}"
        -P "${FREEIMAGE_PREPARE_SCRIPT}"

    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    CONFIGURE_COMMAND ""

    BUILD_COMMAND
        ${CMAKE_MAKE_PROGRAM}
        -f Makefile.gnu
        libfreeimage.a
        "CC=${CMAKE_C_COMPILER}"
        "CXX=${CMAKE_CXX_COMPILER}"
        "AR=${CMAKE_AR}"
        "CFLAGS=${FREEIMAGE_CFLAGS}"
        "CXXFLAGS=${FREEIMAGE_CXXFLAGS}"

    BUILD_IN_SOURCE TRUE

    BUILD_BYPRODUCTS
        "${FREEIMAGE_LIBRARY}"

    INSTALL_COMMAND ""
)

# ------------------------------------------------------------
# Imported library target
# ------------------------------------------------------------

add_library(FreeImageLib STATIC IMPORTED GLOBAL)

set_target_properties(FreeImageLib PROPERTIES
    IMPORTED_LOCATION
        "${FREEIMAGE_LIBRARY}"

    INTERFACE_INCLUDE_DIRECTORIES
        "${FREEIMAGE_HEADER_DIR}"
)

add_dependencies(FreeImageLib FreeImageExternal)

# ------------------------------------------------------------
# Clean support
# ------------------------------------------------------------

set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_CLEAN_FILES
    "${FREEIMAGE_BUILD_SOURCE_DIR}"
    "${FREEIMAGE_EXTERNAL_PREFIX}"
)
