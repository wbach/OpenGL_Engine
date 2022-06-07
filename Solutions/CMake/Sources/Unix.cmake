if(UNIX)
    message(STATUS "Unix : ON")
    include_directories(/usr/include/freetype2)


    if(BUILD_I386)
    else()
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(SDL2 REQUIRED sdl2)
        pkg_check_modules(SDL_net REQUIRED SDL2_net)
        pkg_check_modules(GL REQUIRED gl)
        pkg_check_modules(GLEW REQUIRED glew)
       # pkg_check_modules(ASSIMP REQUIRED assimp)
        #pkg_check_modules(FREEIMAGE REQUIRED freeimage)
        pkg_check_modules(FREETYPE REQUIRED freetype2)
        pkg_check_modules(GLU REQUIRED glu)
    endif()

    if(NOT BUILD_ANDROID)
        add_definitions(-DUSE_GNU)
    endif()

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
