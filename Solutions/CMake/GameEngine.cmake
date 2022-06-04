if(BUILD_GAME_ENGINE)
    link_directories(${CMAKE_CURRENT_BINARY_DIR}/bullet)
    link_directories(${CMAKE_CURRENT_BINARY_DIR}/assimpp)

    set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "")
    set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "")
    set(BUILD_OPENGL3_DEMOS OFF CACHE BOOL "")
    set(USE_GRAPHICAL_BENCHMARK OFF CACHE BOOL "")
    set(USE_GLUT OFF CACHE BOOL "")
    set(BUILD_CLSOCKET OFF CACHE BOOL "")

    set(BUILD_EGL OFF CACHE BOOL "")
    set(BUILD_ENET OFF CACHE BOOL "")
    set(BUILD_EXTRAS OFF CACHE BOOL "")

    #option(ASSIMP_BUILD_TESTS "" OFF)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../Tools/common/bullet/ ${CMAKE_CURRENT_BINARY_DIR}/bullet)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../Tools/common/assimp-5.0.1/ ${CMAKE_CURRENT_BINARY_DIR}/assimp)

    include(./Sources/GameEngineSources.cmake)

    include_directories(
      ${CMAKE_CURRENT_BINARY_DIR}/assimp/include/
  )

    set_source_files_properties(
      ${GameEngineSources}
      PROPERTIES
      COMPILE_FLAGS ${EngineCompileFlags}
    )
    add_library(GameEngineLib ${GameEngineSources})
    #set_target_properties(GameEngineLib PROPERTIES COMPILE_FLAGS ${EngineCompileFlags})

endif()