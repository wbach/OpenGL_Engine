if(BUILD_RPG_GAME_SERVER)

    include(./Sources/RpgGameServerSources.cmake)
    add_executable(RpgGameServer ${RpgGameServerSources})
    if(BUILD_I386)
        set_target_properties(RpgGameServer PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
    endif()
    target_link_libraries(RpgGameServer  UtilsNetworkLib CommonLib UtilsLib ${LinkingLibs} ${BoostLinkingLibs})
endif()
