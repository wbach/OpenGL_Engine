if(BUILD_UTILS_NETWORK)

    # --- Źródła ---
    include(${CMAKE_CURRENT_SOURCE_DIR}/Sources/UtilsNetworkSources.cmake)

    # --- Biblioteka UtilsNetwork ---
    add_library(UtilsNetworkLib SHARED ${UtilsNetworkSources})

    set_target_properties(UtilsNetworkLib PROPERTIES POSITION_INDEPENDENT_CODE ON)

    # Flagi kompilacji (tylko dla tego targetu)
    target_compile_options(UtilsNetworkLib PRIVATE ${ENGINE_COMPILE_FLAGS})

    # Include directories (nagłówki dla innych modułów, jeśli potrzebne)
    target_include_directories(UtilsNetworkLib
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/Sources/UtilsNetwork
    )

    # Linkowanie z innymi bibliotekami (jeśli potrzebne)
    target_link_libraries(UtilsNetworkLib 
        PRIVATE 
            UtilsLib
    )
    if(WIN32)
        copy_dll_to_build_dir(UtilsNetworkLib)
    endif()
endif()
