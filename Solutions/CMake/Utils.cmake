if(BUILD_UTILS)

    # --- Źródła ---
    include(${CMAKE_CURRENT_SOURCE_DIR}/Sources/UtilsSources.cmake)

    # --- Biblioteka utils ---
    add_library(UtilsLib SHARED ${UtilsSources})

    set_target_properties(UtilsLib PROPERTIES POSITION_INDEPENDENT_CODE ON)

    # Flagi kompilacji
    target_compile_options(UtilsLib PRIVATE ${ENGINE_COMPILE_FLAGS})

    # Include directories (jeśli utils ma własne headery)
    target_include_directories(UtilsLib
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/Sources/Utils
    )

    target_link_libraries(UtilsLib
        PUBLIC
            stdc++fs
    )

endif()
