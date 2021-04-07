if(BUILD_GAME_ENGINE OR BUILD_TEST_GAME)
    include(./Sources/OpenGLApiSources.cmake)
    set_source_files_properties(
      ${OpenGLApiSources}
      PROPERTIES
      COMPILE_FLAGS ${EngineCompileFlags}
    )
    add_library(OpenGLApiLib ${OpenGLApiSources})
    #set_target_properties(OpenGLApiLib PROPERTIES COMPILE_FLAGS ${EngineCompileFlags})
endif()