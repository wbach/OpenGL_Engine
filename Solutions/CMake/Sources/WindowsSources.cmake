if(MINGW)
   message(STATUS "MINGW : ON")
    include_directories(
        ../../Tools/Windows/Mingw
        ../../Tools/Windows/rapidxml-1.13/
        ../../Tools/Windows/fbx_sdk/include/
        ../../Tools/Windows/Assimp/include/
        ../../Tools/Windows/SDL2-2.0.3/include/
		../../Tools/Windows/SDL2_net-2.0.1/include/
        ../../Tools/Windows/
        ../../Tools/common/glm/
        ../../Tools/Windows/
        ../../Tools/Windows/freeImage/
        ../../Tools/Windows/freetype/include/
        ../../Tools/Windows/GLFW3/include/
        ../Utils/
        )

    link_directories(
        ../../Tools/Windows/fbx_sdk/lib/vs2015/x86/release/
        ../../Tools/Windows/Assimp/lib/x86/
        ../../Tools/Windows/GL/
        ../../Tools/Windows/SDL2-2.0.3/lib/x86/
		../../Tools/Windows/SDL2_net-2.0.1/lib/x86/
        ../../Tools/Windows/freeImage/
        ../../Tools/Windows/freetype/lib/
        ../../Tools/Windows/GLFW3/lib-vc2015/
        )

        add_definitions(-DUSE_GNU)
        add_definitions(-DUSE_MINGW)

        set(LinkingLibs
            SDL2main
            SDL2
			SDL2_net
            glew32
            glu32
            opengl32
            assimp
            pthread
            freeimage
            freetype
        )
endif()