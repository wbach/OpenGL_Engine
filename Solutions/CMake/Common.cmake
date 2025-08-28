if(BUILD_COMMON)

    # --- Źródła ---
    include(${CMAKE_CURRENT_SOURCE_DIR}/Sources/CommonSources.cmake)

    # --- Biblioteka Common ---
    add_library(CommonLib SHARED ${CommonSources})

    set_target_properties(CommonLib PROPERTIES POSITION_INDEPENDENT_CODE ON)

    # Flagi kompilacji (tylko dla tego targetu)
    target_compile_options(CommonLib PRIVATE ${ENGINE_COMPILE_FLAGS})

    # Include directories (nagłówki dla innych modułów)
    target_include_directories(CommonLib
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/Sources/Common
    )

    # Linkowanie (jeśli potrzebne)
    # target_link_libraries(CommonLib PRIVATE UtilsLib)

endif()
