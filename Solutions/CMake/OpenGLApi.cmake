if(BUILD_GAME_ENGINE OR BUILD_TEST_GAME)
      if(DEFINED ENV{USERROOT})
      include_directories($ENV{USERROOT}/usr/include/freetype2)
      include_directories($ENV{USERROOT}/usr/include/minizip)
    else()
      include_directories(/usr/include/freetype2)
    endif()

    include(./Sources/OpenGLApiSources.cmake)
    # set_source_files_properties(
    #   ${OpenGLApiSources}
    #   PROPERTIES
    #   COMPILE_FLAGS ${EngineCompileFlags}
    # )
    add_library(OpenGLApiLib ${OpenGLApiSources})
    #set_target_properties(OpenGLApiLib PROPERTIES COMPILE_FLAGS ${EngineCompileFlags})
endif()