if(BUILD_GAME_ENGINE OR BUILD_GAME)

    # --- Źródła ---
    include(${CMAKE_CURRENT_SOURCE_DIR}/Sources/InputSources.cmake)

    # --- Biblioteka Input ---
    add_library(InputLib SHARED ${InputSources})

    set_target_properties(InputLib PROPERTIES POSITION_INDEPENDENT_CODE ON)

    # Flagi kompilacji (tylko dla InputLib)
    target_compile_options(InputLib PRIVATE ${ENGINE_COMPILE_FLAGS})

    # Include directories (jeśli Input ma swoje nagłówki)
    target_include_directories(InputLib
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/Sources/Input
    )

endif()
