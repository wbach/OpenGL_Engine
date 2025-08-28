if(BUILD_RPG_GAME_SERVER)
    include(./Sources/RpgGameServerSources.cmake)
    set_source_files_properties(
      ${RpgGameServerSources}
      PROPERTIES
      COMPILE_FLAGS ${ENGINE_COMPILE_FLAGS}
    )
    add_executable(RpgGameServerExe ${RpgGameServerSources})
    #set_target_properties(RpgGameServerExe PROPERTIES COMPILE_FLAGS ${EngineCompileFlags})
    target_link_libraries(RpgGameServerExe  UtilsNetworkLib CommonLib UtilsLib ${LinkingLibs})
endif()
