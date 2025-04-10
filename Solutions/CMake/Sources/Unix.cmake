if(UNIX)
    message(STATUS "Unix : ON")

    if(DEFINED ENV{USERROOT})
      include_directories($ENV{USERROOT}/usr/include/freetype2)
      include_directories($ENV{USERROOT}/usr/include/minizip)
    else()
      include_directories(/usr/include/freetype2)
    endif()

    if(BUILD_I386)
    else()
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(SDL2 REQUIRED sdl2)
        pkg_check_modules(SDL_net REQUIRED SDL2_net)
        pkg_check_modules(GL REQUIRED gl)
        pkg_check_modules(GLEW REQUIRED glew)
        #pkg_check_modules(FREEIMAGE REQUIRED freeimage)
        pkg_check_modules(FREETYPE REQUIRED freetype2)
        pkg_check_modules(GLU REQUIRED glu)

          set(LinkingLibs
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

    add_definitions(-DUSE_GNU)
    link_directories(${PROJECT_BINARY_DIR}/../Tools/linux/fbx/lib/gcc4/x64/release/)
    set(LinkingLibs
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
