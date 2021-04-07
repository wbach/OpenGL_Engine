if(BUILD_AVATAR_GAME)
    option(BUILD_UNIT_TESTS "" OFF)
    include_directories(
    ../../Sources/GameEngine/
    )
    include(./Sources/AvatarSources.cmake)
    set_source_files_properties(
      ${AvatarSources}
      PROPERTIES
      COMPILE_FLAGS ${EngineCompileFlags}
    )
    add_executable(AvatarGameExe ${AvatarSources})
    #set_target_properties(AvatarGameExe PROPERTIES COMPILE_FLAGS ${EngineCompileFlags})
    target_link_libraries(AvatarGameExe GameEngineLib OpenGLApiLib GraphicsApiLib InputLib UtilsNetworkLib CommonLib UtilsLib ${LinkingLibs} ${BulletLinkingLibs} ${BoostLinkingLibs})
endif()