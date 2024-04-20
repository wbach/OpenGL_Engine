if(BUILD_UTILS)
    include(./Sources/UtilsSources.cmake)
    # set_source_files_properties(
    #   ${UtilsSources}
    #   PROPERTIES
    #   COMPILE_FLAGS ${EngineCompileFlags}
    # )
    add_library(UtilsLib ${UtilsSources})
endif()