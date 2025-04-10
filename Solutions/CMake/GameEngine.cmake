if(BUILD_GAME_ENGINE)
    set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "")
    set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "")
    set(BUILD_OPENGL3_DEMOS OFF CACHE BOOL "")
    set(USE_GRAPHICAL_BENCHMARK OFF CACHE BOOL "")
    set(USE_GLUT OFF CACHE BOOL "")
    set(BUILD_CLSOCKET OFF CACHE BOOL "")

    set(BUILD_EGL OFF CACHE BOOL "")
    set(BUILD_ENET OFF CACHE BOOL "")
    set(BUILD_EXTRAS OFF CACHE BOOL "")

    include(./Sources/GameEngineSources.cmake)

    include_directories(
      ${CMAKE_CURRENT_BINARY_DIR}/assimp/include/
     # ${CMAKE_CURRENT_BINARY_DIR}/FreeImage/Source/
  )

    set_source_files_properties(
      ${GameEngineSources}
      PROPERTIES
      COMPILE_FLAGS ${EngineCompileFlags}
    )
    add_library(GameEngineLib ${GameEngineSources})
endif()