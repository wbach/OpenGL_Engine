
include(ExternalProject)

set(FREEIMAGE_SOURCE_DIR
    "${CMAKE_SOURCE_DIR}/../../Tools/common/FreeImage3180"
)

set(FREEIMAGE_LIBRARY
    "${FREEIMAGE_SOURCE_DIR}/libfreeimage.a"
)

ExternalProject_Add(FreeImageExternal
    SOURCE_DIR "${FREEIMAGE_SOURCE_DIR}"

    CONFIGURE_COMMAND ""

    BUILD_COMMAND
        ${CMAKE_MAKE_PROGRAM} -f Makefile.gnu libfreeimage.a
        CC=${CMAKE_C_COMPILER}
        CXX=${CMAKE_CXX_COMPILER}
        AR=${CMAKE_AR}
        "CFLAGS=-O2 -pthread -fexceptions -fvisibility=hidden -DOPJ_STATIC -DNO_LCMS -DDISABLE_PERF_MEASUREMENT -D__ANSI__ -I. -ISource -ISource/Metadata -ISource/FreeImageToolkit -ISource/LibJPEG -ISource/LibPNG -ISource/LibTIFF4 -ISource/ZLib -ISource/LibOpenJPEG -ISource/OpenEXR -ISource/OpenEXR/Half -ISource/OpenEXR/Iex -ISource/OpenEXR/IlmImf -ISource/OpenEXR/IlmThread -ISource/OpenEXR/Imath -ISource/OpenEXR/IexMath -ISource/LibRawLite -ISource/LibRawLite/dcraw -ISource/LibRawLite/internal -ISource/LibRawLite/libraw -ISource/LibRawLite/src -ISource/LibWebP -ISource/LibJXR -ISource/LibJXR/common/include -ISource/LibJXR/image/sys -ISource/LibJXR/jxrgluelib"
        "CXXFLAGS=-O2 -pthread -fexceptions -std=c++14  -fvisibility=hidden -D__ANSI__ -I. -ISource -ISource/Metadata -ISource/FreeImageToolkit -ISource/LibJPEG -ISource/LibPNG -ISource/LibTIFF4 -ISource/ZLib -ISource/LibOpenJPEG -ISource/OpenEXR -ISource/OpenEXR/Half -ISource/OpenEXR/Iex -ISource/OpenEXR/IlmImf -ISource/OpenEXR/IlmThread -ISource/OpenEXR/Imath -ISource/OpenEXR/IexMath -ISource/LibRawLite -ISource/LibRawLite/dcraw -ISource/LibRawLite/internal -ISource/LibRawLite/libraw -ISource/LibRawLite/src -ISource/LibWebP -ISource/LibJXR -ISource/LibJXR/common/include -ISource/LibJXR/image/sys -ISource/LibJXR/jxrgluelib"

    BUILD_IN_SOURCE TRUE

    INSTALL_COMMAND ""
)

add_library(FreeImageLib STATIC IMPORTED GLOBAL)

set_target_properties(FreeImageLib PROPERTIES
    IMPORTED_LOCATION "${FREEIMAGE_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${FREEIMAGE_SOURCE_DIR}/Source"
)

add_dependencies(FreeImageLib FreeImageExternal)
