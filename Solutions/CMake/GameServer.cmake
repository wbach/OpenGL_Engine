if(BUILD_RPG_GAME_SERVER)

    include(./Sources/RpgGameServerSources.cmake)
    add_executable(RpgGameServerExe ${RpgGameServerSources})
    if(BUILD_I386)
        set_target_properties(RpgGameServerExe PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
    endif()
    target_link_libraries(RpgGameServerExe  UtilsNetworkLib CommonLib UtilsLib ${LinkingLibs} ${BoostLinkingLibs})
endif()
