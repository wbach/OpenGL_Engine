if(BUILD_UTILS_NETWORK_TESTS)



    include(./Sources/UtilsNetworkTestsSources.cmake)
    set_source_files_properties(
      ${UtilsNetworkTestsSources}
      PROPERTIES
      COMPILE_FLAGS ${EngineCompileFlags}
    )
    add_executable(UtilsNetworkTestsExe ${UtilsNetworkTestsSources})
    #set_target_properties(UtilsNetworkTestsExe PROPERTIES COMPILE_FLAGS ${EngineCompileFlags})
    target_link_libraries(UtilsNetworkTestsExe UtilsNetworkLib UtilsLib gtest gtest_main gmock gmock_main ${LinkingLibs})
endif()

