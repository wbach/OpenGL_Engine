if(WIN32 AND (BUILD_GAME_ENGINE OR BUILD_GAME))

    # --- Źródła ---
    include(${CMAKE_CURRENT_SOURCE_DIR}/Sources/DirectXApiSources.cmake)

    # --- Biblioteka DirectXApi --- 
    if(MSVC)
        add_library(DirectXApiLib STATIC ${DirectXApiSources})
    else()
        add_library(DirectXApiLib SHARED ${DirectXApiSources})
    endif()

    set_target_properties(DirectXApiLib PROPERTIES POSITION_INDEPENDENT_CODE ON)
    
    target_compile_definitions(DirectXApiLib PRIVATE ENGINE_EXPORTS)

    # Flagi kompilacji
    target_compile_options(DirectXApiLib PRIVATE ${ENGINE_COMPILE_FLAGS})

    # Include directories
    target_include_directories(DirectXApiLib
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/Sources/DirectXApi
    )

    # Linkowanie bibliotek DirectX 11
    target_link_libraries(DirectXApiLib
        PUBLIC
            d3d11
            dxgi
            d3dcompiler
            InputLib
            UtilsLib
            GraphicsApiLib
    )

    # Kopiowanie DLL dla MinGW
    if(NOT MSVC)
        copy_dll_to_build_dir(DirectXApiLib)
    endif()

endif()
