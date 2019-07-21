if(BUILD_TESTS)
    include_directories(
        ../../Sources/GameEngine/
    )

    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../Tools/common/googletest ${CMAKE_CURRENT_BINARY_DIR}/googletest)
    include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

    include(./Sources/GameEngineTestsSources.cmake)
    add_executable(GAME_ENGINE_TESTS ${GameEngineTestsSources})
    if(BUILD_I386)
        set_target_properties(GAME_ENGINE_TESTS PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
    endif()
    target_link_libraries(GAME_ENGINE_TESTS GameEngineLib OpenGLApiLib InputLib UtilsNetworkLib CommonLib UtilsLib gtest gtest_main gmock gmock_main fbxsdk ${LinkingLibs} ${BoostLinkingLibs})
endif()