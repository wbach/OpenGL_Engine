if(BUILD_GAME_ENGINE OR BUILD_TEST_GAME)
    include(./Sources/VulkanApiSources.cmake)
    set_source_files_properties(
      ${VulkanApiSources}
      PROPERTIES
      COMPILE_FLAGS ${EngineCompileFlags}
    )
    add_library(VulkanApiLib ${VulkanApiSources})
endif()