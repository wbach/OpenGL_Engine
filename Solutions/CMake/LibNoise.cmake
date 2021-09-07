if(BUILD_LIB_NOISE OR BUILD_TERRAIN_GENERATOR)
    include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/../../Tools/libnoise/
        ${CMAKE_CURRENT_SOURCE_DIR}/../../Tools/common/libnoise/src/
        )
    include(./Sources/LibNoiseSources.cmake)
    add_library(LibNoise ${LibNoiseSources})
endif()