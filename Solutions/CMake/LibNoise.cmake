if(BUILD_LIB_NOISE OR BUILD_TERRAIN_GENERATOR)
    include_directories(
        ../../Tools/common/libnoise/
        ../../Tools/common/libnoise/src/
        )
    include(./Sources/LibNoiseSources.cmake)
    add_library(LibNoise ${LibNoiseSources})
endif()