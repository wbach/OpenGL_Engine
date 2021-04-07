if(BUILD_GAME_ENGINE OR BUILD_TEST_GAME)
    include(./Sources/GraphicsApiSources.cmake)
    set_source_files_properties(
      ${GraphicsApiSources}
      PROPERTIES
      COMPILE_FLAGS ${EngineCompileFlags}
    )
    add_library(GraphicsApiLib ${GraphicsApiSources})
    #set_target_properties(GraphicsApiLib PROPERTIES COMPILE_FLAGS ${EngineCompileFlags})
endif()