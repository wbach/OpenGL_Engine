if(UNIX)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2 REQUIRED sdl2)
    pkg_check_modules(GL REQUIRED gl)
    pkg_check_modules(GLEW REQUIRED glew)
    pkg_check_modules(ASSIMP REQUIRED assimp)
    #pkg_check_modules(FREEIMAGE REQUIRED freeimage)
    pkg_check_modules(FREETYPE REQUIRED freetype2)
    pkg_check_modules(GLU REQUIRED glu)

    set(LinkingLibs
        SDL2main
        SDL2
        GL
        GLU
        GLEW
        drm
        assimp
        pthread
        freeimage
        freetype
    )

endif()