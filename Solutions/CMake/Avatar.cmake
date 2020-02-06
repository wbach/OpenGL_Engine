if(BUILD_AVATAR_GAME)
    option(BUILD_UNIT_TESTS "" OFF)
    include_directories(
    ../../Sources/GameEngine/
    )
    include(./Sources/AvatarSources.cmake)
    add_executable(AvatarGameExe ${AvatarSources})
    if(BUILD_I386)
        set_target_properties(AvatarGameExe PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
    endif()
    target_link_libraries(AvatarGameExe GameEngineLib OpenGLApiLib InputLib UtilsNetworkLib CommonLib UtilsLib ${LinkingLibs} ${BulletLinkingLibs} ${BoostLinkingLibs})
endif()