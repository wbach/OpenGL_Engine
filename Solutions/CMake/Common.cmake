#if(BUILD_COMMON)
    include(./Sources/CommonSources.cmake)
    # set_source_files_properties(
    #   ${CommonSources}
    #   PROPERTIES
    #   COMPILE_FLAGS ${EngineCompileFlags}
    # )
    add_library(CommonLib ${CommonSources})
    #set_target_properties(CommonLib PROPERTIES COMPILE_FLAGS ${EngineCompileFlags})
#endif()