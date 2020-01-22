if(BUILD_UTILS_NETWORK_TESTS)
    if(NOT DEFINED GTEST_INLCUDED)
        set(GTEST_INLCUDED true)
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../Tools/common/googletest ${CMAKE_CURRENT_BINARY_DIR}/googletest)
        include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
    endif()

    include(./Sources/UtilsNetworkTestsSources.cmake)
    add_executable(UTILS_NETWORK_TESTS ${UtilsNetworkTestsSources})
    
    if(BUILD_I386)
        set_target_properties(UTILS_NETWORK_TESTS PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
    endif()

    target_link_libraries(UTILS_NETWORK_TESTS UtilsNetworkLib UtilsLib gtest gtest_main gmock gmock_main ${LinkingLibs} ${BoostLinkingLibs})
endif()

