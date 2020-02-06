if(BUILD_TEST_GAME)
    option(BUILD_UNIT_TESTS "" OFF)
    include_directories(
    ../../Sources/GameEngine/
    )
    include(./Sources/TestGameSources.cmake)
    add_executable(TestGlGameExe ${TestGameSources})
    if(BUILD_I386)
        set_target_properties(TestGlGameExe PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
    endif()
    target_link_libraries(TestGlGameExe GameEngineLib OpenGLApiLib InputLib UtilsNetworkLib CommonLib UtilsLib ${LinkingLibs} ${BulletLinkingLibs} ${BoostLinkingLibs})
endif()