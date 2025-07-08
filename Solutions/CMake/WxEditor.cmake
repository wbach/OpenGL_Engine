if(BUILD_WX_EDITOR)
    option(BUILD_UNIT_TESTS "" OFF)

    find_package(wxWidgets REQUIRED gl core base)
    include(${wxWidgets_USE_FILE})

    include_directories(
    ../../Sources/GameEngine/
    )
    include(./Sources/WxEditorSources.cmake)
    set_source_files_properties(
      ${WxEditorSources}
      PROPERTIES
      COMPILE_FLAGS ${EngineCompileFlags}
    )
    add_executable(WxEditorExe ${WxEditorSources})
    #set_target_properties(EditorExe PROPERTIES COMPILE_FLAGS ${EngineCompileFlags})
    target_link_libraries(WxEditorExe GameEngineLib OpenGLApiLib GraphicsApiLib InputLib UtilsNetworkLib CommonLib UtilsLib ${wxWidgets_LIBRARIES} ${LinkingLibs} ${BulletLinkingLibs})
endif()