if(BUILD_GAME_ENGINE OR BUILD_GAME)

    # --- Źródła ---
    include(${CMAKE_CURRENT_SOURCE_DIR}/Sources/OpenGLApiSources.cmake)

    # --- Biblioteka OpenGLApi ---
    add_library(OpenGLApiLib SHARED ${OpenGLApiSources})

    set_target_properties(OpenGLApiLib PROPERTIES POSITION_INDEPENDENT_CODE ON)

    # Flagi kompilacji (tylko dla tego targetu)
    target_compile_options(OpenGLApiLib PRIVATE ${ENGINE_COMPILE_FLAGS})

    # Include directories (nagłówki API OpenGL, jeśli są w projekcie)
    target_include_directories(OpenGLApiLib
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/Sources/OpenGLApi
    )

    # Linkowanie (przykład: systemowe OpenGL + inne moduły)
    find_package(OpenGL REQUIRED)
    target_link_libraries(OpenGLApiLib
        PUBLIC
            OpenGL::GL
            InputLib
            UtilsLib
            GraphicsApiLib
    )

    if(WIN32)
        copy_dll_to_build_dir(OpenGLApiLib)
    endif()

endif()
