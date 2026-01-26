if(BUILD_GAME_ENGINE OR BUILD_GAME)

    # --- Źródła ---
    include(${CMAKE_CURRENT_SOURCE_DIR}/Sources/GraphicsApiSources.cmake)

    
    if(MSVC)
        add_library(GraphicsApiLib STATIC ${GraphicsApiSources})
    else()
        add_library(GraphicsApiLib SHARED ${GraphicsApiSources})
    endif()

    set_target_properties(GraphicsApiLib PROPERTIES POSITION_INDEPENDENT_CODE ON)

    # Flagi kompilacji
    target_compile_options(GraphicsApiLib PRIVATE ${ENGINE_COMPILE_FLAGS})

    # Include directories (jeśli są nagłówki do eksportu)
    target_include_directories(GraphicsApiLib
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/Sources/GraphicsApi
    )
    if(WIN32)
        copy_dll_to_build_dir(GraphicsApiLib)
    endif()
endif()
