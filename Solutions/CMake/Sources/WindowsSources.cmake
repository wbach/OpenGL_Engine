if(MINGW)
   message(STATUS "MINGW : ON")
    include_directories(
        ../../Sources/
        ../../Sources/Utils
        ../../Sources/OpenGLApi
        ../../Sources/GraphicsApi
        ../../Tools/Windows
        ../../Tools/common/glm
        ../../Tools/common/bullet/src/
        ../../Tools/common/rapidxml-1.13
        ../../Tools/Windows/assimp-5.0.1/include
        ../../Tools/Windows/SDL2-2.0.12/include
        ../../Tools/Windows/SDL2-2.0.12/include/SDL2
        ../../Tools/Windows/SDL2_net-2.0.1/include
        ../../Tools/Windows/SDL2_ttf-2.0.15/include
        ../../Tools/Windows/SDL2_image-2.0.4/include
        ../../Tools/Windows/glew-2.1.0/include
        ../../Tools/Windows/freeImage
        ../../Tools/Windows/freetype/include
        ../../Tools/Windows/GLFW3/include
#        ../../Tools/Windows/gtest/include
 #       ../../Tools/Windows/gmock/include
        ../../Tools/Windows/Directx/Include
        )

    link_directories(
        ../../Tools/Windows/fbx_sdk/lib/vs2017/x64/release
        ../../Tools/Windows/assimp-5.0.1/lib/x64/release
        ../../Tools/Windows/GL
        ../../Tools/Windows/glew-2.1.0/lib/Release/x64
        ../../Tools/Windows/SDL2_ttf-2.0.15/lib/x64
        ../../Tools/Windows/SDL2-2.0.12/lib/x64
        ../../Tools/Windows/SDL2_net-2.0.1/lib/x64
        ../../Tools/Windows/SDL2_image-2.0.4/lib/x64
        ../../Tools/Windows/freeImage/x64
        ../../Tools/Windows/freetype/win64
        ../../Tools/Windows/GLFW3/lib-vc2015
  #      ../../Tools/Windows/gtest/lib/x64/Release
  #      ../../Tools/Windows/gmock/lib/x64/Release
        ../../Tools/Windows/Directx/Lib/x64
        ../../Tools/Windows/bullet/x64/Release
        )

       # add_definitions(-DUSE_GNU)
        add_definitions(-DUSE_MINGW)

        set(LinkingLibs
            SDL2main
            SDL2
			SDL2_net
            SDL2_ttf
            glew32
            glu32
            opengl32
            assimp
            pthread
            freeimage
            freetype
        )

#    if(BUILD_GAME_ENGINE OR BUILD_TEST_GAME)
 #       include(./Sources/DirectXApiSources.cmake)
 #       set_source_files_properties(
 #         ${DirectXApiSources}
 #         PROPERTIES
 #         COMPILE_FLAGS ${EngineCompileFlags}
 #       )
 #       add_library(DirectXApiLib ${DirectXApiSources})
 #       #set_target_properties(DirectXApiLib PROPERTIES COMPILE_FLAGS ${EngineCompileFlags})
 #   endif()
endif()