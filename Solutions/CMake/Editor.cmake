if(BUILD_EDITOR)
    option(BUILD_UNIT_TESTS "" OFF)

    include_directories(
    ../../Sources/GameEngine/
    )
    include(./Sources/EditorSources.cmake)
    set_source_files_properties(
      ${EditorSources}
      PROPERTIES
      COMPILE_FLAGS ${EngineCompileFlags}
    )
    add_executable(EditorExe ${EditorSources})
    #set_target_properties(EditorExe PROPERTIES COMPILE_FLAGS ${EngineCompileFlags})
    target_link_libraries(EditorExe GameEngineLib OpenGLApiLib GraphicsApiLib InputLib UtilsNetworkLib CommonLib UtilsLib ${LinkingLibs} ${BulletLinkingLibs})
endif()