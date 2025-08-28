if(BUILD_GAME_ENGINE)

    # --- Third-party: Bullet & Assimp ---
    set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
    set(BUILD_OPENGL3_DEMOS OFF CACHE BOOL "" FORCE)
    set(USE_GRAPHICAL_BENCHMARK OFF CACHE BOOL "" FORCE)
    set(USE_GLUT OFF CACHE BOOL "" FORCE)
    set(BUILD_CLSOCKET OFF CACHE BOOL "" FORCE)
    set(BUILD_EGL OFF CACHE BOOL "" FORCE)
    set(BUILD_ENET OFF CACHE BOOL "" FORCE)
    set(BUILD_EXTRAS OFF CACHE BOOL "" FORCE)

    # Dodaj subprojekty (budowane razem z silnikiem)
    set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)

    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../Tools/common/bullet
                     ${CMAKE_CURRENT_BINARY_DIR}/bullet)

    set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../Tools/common/assimp-5.0.1
                     ${CMAKE_CURRENT_BINARY_DIR}/assimp)

    # --- Źródła silnika ---
    include(${CMAKE_CURRENT_SOURCE_DIR}/Sources/GameEngineSources.cmake)

    # --- Biblioteka silnika ---
    add_library(GameEngineLib SHARED ${GameEngineSources})

    set_target_properties(GameEngineLib PROPERTIES POSITION_INDEPENDENT_CODE ON)
    # Flagi kompilacji dla silnika
    target_compile_options(GameEngineLib PRIVATE ${ENGINE_COMPILE_FLAGS})

    # Include directories dla silnika
    target_include_directories(GameEngineLib
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/Sources
            ${CMAKE_CURRENT_BINARY_DIR}/assimp/include
            # ${CMAKE_CURRENT_BINARY_DIR}/FreeImage/Source/   # jeśli kiedyś użyjesz
    )

    # Linkowanie zewnętrznych bibliotek
    target_link_libraries(GameEngineLib
        PRIVATE
            ${BULLET_LIBS}
            assimp
    )

endif()
