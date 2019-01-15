if(BUILD_GAME_ENGINE OR BUILD_TEST_GAME)
    include(./Sources/GameEngineSources.cmake)
    add_library(GameEngineLib ${GameEngineSources})
    if(BUILD_I386)
        set_target_properties(GameEngineLib PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
    endif()
endif()