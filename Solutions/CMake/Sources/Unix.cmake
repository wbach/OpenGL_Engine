if(UNIX)
    message(STATUS "Unix : ON")

    # --- Include directories ---
    if(DEFINED ENV{USERROOT})
        include_directories(
            $ENV{USERROOT}/usr/include/freetype2
            $ENV{USERROOT}/usr/include/minizip
        )
    else()
        include_directories(/usr/include/freetype2)
    endif()

    add_definitions(-DUSE_GNU)

    # --- Biblioteki do linkowania ---
    if(NOT BUILD_I386)
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(SDL2 REQUIRED sdl2)
        pkg_check_modules(SDL_net REQUIRED SDL2_net)
        pkg_check_modules(GL REQUIRED gl)
        pkg_check_modules(GLEW REQUIRED glew)
        pkg_check_modules(FREETYPE REQUIRED freetype2)
        pkg_check_modules(GLU REQUIRED glu)

        set(UNIX_LINK_LIBS
            SDL2main
            SDL2
            SDL2_net
            SDL2_ttf
            GL
            GLU
            GLEW
            drm
            assimp
            pthread
            stdc++fs
            freeimage
            freetype
        )
    endif()

    # --- Link directories (jeśli potrzeba dla FBX) ---
    link_directories(${PROJECT_BINARY_DIR}/../Tools/linux/fbx/lib/gcc4/x64/release/)
endif()
