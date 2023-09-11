if(BUILD_TESTS)
    include_directories(
        ../../Sources/GameEngine/
    )

    if(NOT DEFINED GTEST_INLCUDED)
        set(GTEST_INLCUDED true)
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../Tools/common/gtest ${CMAKE_CURRENT_BINARY_DIR}/googletest)
        include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
    endif()

    include(./Sources/GameEngineTestsSources.cmake)
    set_source_files_properties(
      ${GameEngineTestsSources}
      PROPERTIES
      COMPILE_FLAGS ${EngineCompileFlags}
    )
    add_executable(GameEngineTestsExe ${GameEngineTestsSources})
    #set_target_properties(GameEngineTestsExe PROPERTIES COMPILE_FLAGS ${EngineCompileFlags})
    target_link_libraries(GameEngineTestsExe GameEngineLib OpenGLApiLib GraphicsApiLib InputLib UtilsNetworkLib CommonLib UtilsLib gtest gtest_main gmock gmock_main ${LinkingLibs} ${BulletLinkingLibs})
endif()
