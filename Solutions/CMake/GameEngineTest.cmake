if(BUILD_TESTS)
    include_directories(
        ../../Sources/GameEngine/
    )

    if(NOT DEFINED GTEST_INLCUDED)
        set(GTEST_INLCUDED true)
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../Tools/common/googletest ${CMAKE_CURRENT_BINARY_DIR}/googletest)
        include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
    endif()

    include(./Sources/GameEngineTestsSources.cmake)
    add_executable(GameEngineTestsExe ${GameEngineTestsSources})

    if(BUILD_I386)
        set_target_properties(GameEngineTestsExe PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
    endif()

    target_link_libraries(GameEngineTestsExe GameEngineLib OpenGLApiLib InputLib UtilsNetworkLib CommonLib UtilsLib gtest gtest_main gmock gmock_main fbxsdk ${LinkingLibs} ${BoostLinkingLibs})
endif()
