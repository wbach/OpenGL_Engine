if(BUILD_GAME_ENGINE OR BUILD_GAME)

    # --- Źródła ---
    include(${CMAKE_CURRENT_SOURCE_DIR}/Sources/GraphicsApiSources.cmake)

    # --- Biblioteka GraphicsApi ---
    add_library(GraphicsApiLib SHARED ${GraphicsApiSources})

    set_target_properties(GraphicsApiLib PROPERTIES POSITION_INDEPENDENT_CODE ON)

    # Flagi kompilacji
    target_compile_options(GraphicsApiLib PRIVATE ${ENGINE_COMPILE_FLAGS})

    # Include directories (jeśli są nagłówki do eksportu)
    target_include_directories(GraphicsApiLib
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/Sources/GraphicsApi
    )

endif()
