if(BUILD_GAME_ENGINE)
    link_directories(${CMAKE_CURRENT_BINARY_DIR}/bullet)
    link_directories(${CMAKE_CURRENT_BINARY_DIR}/assimpp)
    link_directories(${PROJECT_BINARY_DIR}/../Tools/linux/fbx/lib/gcc4/x64/release)

    add_subdirectory(${PROJECT_BINARY_DIR}/../Tools/common/bullet/ ${CMAKE_CURRENT_BINARY_DIR}/bullet)
    add_subdirectory(${PROJECT_BINARY_DIR}/../Tools/common/assimp-5.0.1/ ${CMAKE_CURRENT_BINARY_DIR}/assimp)

    include(./Sources/GameEngineSources.cmake)
    add_library(GameEngineLib ${GameEngineSources})
    if(BUILD_I386)
        set_target_properties(GameEngineLib PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
    endif()
endif()