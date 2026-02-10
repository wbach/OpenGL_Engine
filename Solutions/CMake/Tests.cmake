if(BUILD_TESTS)

    # --- Definicje kompilatora ---
    #add_definitions(-DNOREALTIME_LOG_ENABLED)

    # --- Include directories ---
    set(TEST_INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine
        ${CMAKE_CURRENT_SOURCE_DIR}/Sources/Tests
    )

    # --- GoogleTest ---
    include(FetchContent)
    FetchContent_Declare(
      googletest
      URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    # --- Źródła testów ---
    include(${CMAKE_CURRENT_SOURCE_DIR}/Sources/TestsSources.cmake)

    # --- Executable testowy ---
    add_executable(GameEngineTestsExe ${TestsSources})

    # Flagi kompilacji
    target_compile_options(GameEngineTestsExe PRIVATE ${ENGINE_COMPILE_FLAGS})

    # Include directories
    target_include_directories(GameEngineTestsExe PRIVATE ${TEST_INCLUDE_DIRS})

    # Linkowanie bibliotek i targetów
    target_link_libraries(GameEngineTestsExe
        PRIVATE
            GameEngineLib
            OpenGLApiLib
            GraphicsApiLib
            InputLib
            UtilsNetworkLib
            CommonLib
            UtilsLib
            gtest
            gtest_main
            gmock
            gmock_main
            ${UNIX_LINK_LIBS}
            ${BULLET_LIBS}
    )

endif()
