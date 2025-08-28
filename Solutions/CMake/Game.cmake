if(BUILD_GAME)

    # --- Opcje testów (jeśli używane) ---
    option(BUILD_UNIT_TESTS "" OFF)

    # --- Include directories ---
    # Przypięte do targetu, nie globalnie
    set(GAME_INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine
        ${CMAKE_CURRENT_SOURCE_DIR}/Sources/Game
    )

    # --- Źródła ---
    include(${CMAKE_CURRENT_SOURCE_DIR}/Sources/GameSources.cmake)

    # --- Executable ---
    add_executable(GameExe ${GameSources})

    # Flagi kompilacji
    target_compile_options(GameExe PRIVATE ${ENGINE_COMPILE_FLAGS})

    # Include directories
    target_include_directories(GameExe PRIVATE ${GAME_INCLUDE_DIRS})

    # Linkowanie bibliotek i targetów
    target_link_libraries(GameExe
        PUBLIC
            GameEngineLib
            OpenGLApiLib
            GraphicsApiLib
            InputLib
            UtilsNetworkLib
            CommonLib
            UtilsLib
            ${BULLET_LIBS}
            ${UNIX_LINK_LIBS}
    )

endif()
