if(BUILD_TEST_GAME)
    option(BUILD_UNIT_TESTS "" OFF)
    include_directories(
    ../../Sources/GameEngine/
    )
    include(./Sources/TestGameSources.cmake)
    add_executable(TestGlGameExe ${TestGameSources})
    set_target_properties(TestGlGameExe PROPERTIES COMPILE_FLAGS ${EngineCompileFlags})
    target_link_libraries(TestGlGameExe GameEngineLib OpenGLApiLib GraphicsApiLib InputLib UtilsNetworkLib CommonLib UtilsLib ${LinkingLibs} ${BulletLinkingLibs})
endif()