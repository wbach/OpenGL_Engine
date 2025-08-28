if(BUILD_UTILS_NETWORK_TESTS)

    # --- Źródła testów ---
    include(${CMAKE_CURRENT_SOURCE_DIR}/Sources/UtilsNetworkTestsSources.cmake)

    # --- Executable testowy ---
    add_executable(UtilsNetworkTestsExe ${UtilsNetworkTestsSources})

    # Flagi kompilacji
    target_compile_options(UtilsNetworkTestsExe PRIVATE ${ENGINE_COMPILE_FLAGS})

    # Include directories (dla nagłówków UtilsNetwork i Utils)
    target_include_directories(UtilsNetworkTestsExe
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/Sources/UtilsNetwork
            ${CMAKE_CURRENT_SOURCE_DIR}/Sources/Utils
    )

    # Linkowanie z bibliotekami testowymi i modułami
    target_link_libraries(UtilsNetworkTestsExe
        PRIVATE
            UtilsNetworkLib
            UtilsLib
            gtest
            gtest_main
            gmock
            gmock_main
            ${UNIX_LINK_LIBS}
    )

endif()
