if(BUILD_EDITOR)
    option(BUILD_UNIT_TESTS "" OFF)

    include_directories(
    ../../Sources/GameEngine/
    )
    include(./Sources/EditorSources.cmake)
    add_executable(Editor ${EditorSources})
    if(BUILD_I386)
        set_target_properties(Editor PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
    endif()
    target_link_libraries(Editor GameEngineLib OpenGLApiLib InputLib UtilsNetworkLib CommonLib UtilsLib ${LinkingLibs} ${BulletLinkingLibs} ${BoostLinkingLibs})
endif()