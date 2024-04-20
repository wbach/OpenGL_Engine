if(BUILD_GAME_ENGINE OR BUILD_TEST_GAME)
    include(./Sources/InputSources.cmake)
    # set_source_files_properties(
    #   ${InputSources}
    #   PROPERTIES
    #   COMPILE_FLAGS ${EngineCompileFlags}
    # )
    add_library(InputLib ${InputSources})
    #set_target_properties(InputLib PROPERTIES COMPILE_FLAGS ${EngineCompileFlags})
endif()