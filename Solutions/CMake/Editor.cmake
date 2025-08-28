if(BUILD_EDITOR)

    # --- Opcje testów (jeśli używane) ---
    option(BUILD_UNIT_TESTS "" OFF)

    # --- Include directories (przypięte do targetu) ---
    set(EDITOR_INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine
        ${CMAKE_CURRENT_SOURCE_DIR}/Sources/Editor
    )

    # --- Źródła ---
    include(${CMAKE_CURRENT_SOURCE_DIR}/Sources/EditorSources.cmake)

    # --- Executable ---
    add_executable(EditorExe ${EditorSources})

    # Flagi kompilacji
    target_compile_options(EditorExe PRIVATE ${ENGINE_COMPILE_FLAGS})

    # Include directories
    target_include_directories(EditorExe PRIVATE ${EDITOR_INCLUDE_DIRS})

    # Linkowanie bibliotek i targetów
    target_link_libraries(EditorExe
        PRIVATE
            GameEngineLib
            OpenGLApiLib
            GraphicsApiLib
            InputLib
            UtilsNetworkLib
            CommonLib
            UtilsLib
            ${UNIX_LINK_LIBS}
            ${BULLET_LIBS}
    )

endif()
