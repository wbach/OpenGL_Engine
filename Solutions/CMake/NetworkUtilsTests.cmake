if(BUILD_UtilsNetworkTestsExe)

    if(NOT DEFINED GTEST_INLCUDED)
        set(GTEST_INLCUDED true)
        
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../Tools/common/googletest ${CMAKE_CURRENT_BINARY_DIR}/googletest)
        include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

        message(STATUS "gtest: ${CMAKE_CURRENT_SOURCE_DIR}/../../Tools/common/googletest ${CMAKE_CURRENT_BINARY_DIR}/googletest}")
        message(STATUS "gtest: ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR}")
    endif()

    include(./Sources/UtilsNetworkTestsSources.cmake)
    add_executable(UtilsNetworkTestsExe ${UtilsNetworkTestsSources})
    
    if(BUILD_I386)
        set_target_properties(UtilsNetworkTestsExe PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
    endif()

    target_link_libraries(UtilsNetworkTestsExe UtilsNetworkLib UtilsLib gtest gtest_main gmock gmock_main ${LinkingLibs} ${BoostLinkingLibs})
endif()

