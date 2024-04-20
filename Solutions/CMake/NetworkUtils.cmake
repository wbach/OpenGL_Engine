#if(BUILD_UTILS_NETWORK)
    include(./Sources/UtilsNetworkSources.cmake)
    # set_source_files_properties(
    #   ${UtilsNetworkSources}
    #   PROPERTIES
    #   COMPILE_FLAGS ${EngineCompileFlags}
    # )
    add_library(UtilsNetworkLib ${UtilsNetworkSources})
    #set_target_properties(UtilsNetworkLib PROPERTIES COMPILE_FLAGS ${EngineCompileFlags})
#endif()