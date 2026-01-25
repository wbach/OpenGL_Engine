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
if(WIN32)
    target_compile_definitions(GameExe PRIVATE SDL_MAIN_HANDLED)
    set_target_properties(GameExe PROPERTIES WIN32_EXECUTABLE OFF)
endif()


    # Flagi kompilacji
    target_compile_options(GameExe PRIVATE ${ENGINE_COMPILE_FLAGS})

    
    if(WIN32)
        target_link_options(GameExe PRIVATE -Wl,-subsystem,console)
    endif()

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

    # katalog z narzędziami względem CMakeLists.txt
    set(TOOLS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../Tools/Dlls")

    if(WIN32)
        add_custom_command(TARGET GameExe POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${TOOLS_DIR}"
                "${CMAKE_BINARY_DIR}"
        )
    endif()

endif()
