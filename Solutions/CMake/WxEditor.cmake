if(BUILD_WX_EDITOR)

    # --- Opcje testów (jeśli używane) ---
    option(BUILD_UNIT_TESTS "" OFF)

    # --- wxWidgets ---
    find_package(wxWidgets REQUIRED gl core base)
    include(${wxWidgets_USE_FILE})

    # --- Źródła ---
    include(${CMAKE_CURRENT_SOURCE_DIR}/Sources/WxEditorSources.cmake)

    # --- Executable ---
    add_executable(WxEditorExe ${WxEditorSources})

    # Flagi kompilacji
    target_compile_options(WxEditorExe PRIVATE ${ENGINE_COMPILE_FLAGS})

    # Include directories
    target_include_directories(WxEditorExe
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine
            ${CMAKE_CURRENT_SOURCE_DIR}/Sources/WxEditor
    )

    # Linkowanie bibliotek i targetów
    target_link_libraries(WxEditorExe
        PUBLIC
            GameEngineLib
            OpenGLApiLib
            GraphicsApiLib
            InputLib
            UtilsNetworkLib
            CommonLib
            UtilsLib
            ${wxWidgets_LIBRARIES}
            ${BULLET_LIBS}
            ${UNIX_LINK_LIBS}
    )

endif()
