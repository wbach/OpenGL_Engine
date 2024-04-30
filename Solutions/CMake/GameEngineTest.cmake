if(BUILD_TESTS)
    add_definitions(-DNOREALTIME_LOG_ENABLED)
    include_directories(
        ../../Sources/GameEngine/
    )

    include(FetchContent)
    FetchContent_Declare(
      googletest
      URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

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
