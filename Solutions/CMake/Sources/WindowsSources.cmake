include_directories(
    ../../Sources/
    ../../Sources/Utils
    ../../Sources/OpenGLApi
    ../../Sources/GraphicsApi
    ../../Tools/Windows
    ../../Tools/common/glm
    ../../Tools/common/rapidxml-1.13
    ../../Tools/Windows/SDL2-2.0.12/include
    ../../Tools/Windows/SDL2-2.0.12/include/SDL2
    ../../Tools/Windows/SDL2_net-2.0.1/include
    ../../Tools/Windows/SDL2_ttf-2.0.15/include
    ../../Tools/Windows/SDL2_image-2.0.4/include
    ../../Tools/Windows/glew-2.1.0/include
    ../../Tools/Windows/freeImage
    ../../Tools/Windows/freetype/include
    ../../Tools/Windows/GLFW3/include
)

link_directories(
    ../../Tools/Windows/GL
    ../../Tools/Windows/glew-2.1.0/lib/Release/x64
    ../../Tools/Windows/SDL2_ttf-2.0.15/lib/x64
    ../../Tools/Windows/SDL2-2.0.12/lib/x64
    ../../Tools/Windows/SDL2_net-2.0.1/lib/x64
    ../../Tools/Windows/SDL2_image-2.0.4/lib/x64
    ../../Tools/Windows/freeImage/x64
    ../../Tools/Windows/freetype/win64
)

set(BASE_LINK_LIBS
    SDL2main
    SDL2
    SDL2_net
    SDL2_ttf
    glew32
    glu32
    opengl32
    FreeImage
    freetype
)

if(MSVC)
    message(STATUS "MSVC : ON")
    
    link_directories(
        ../../Tools/Windows/fbx_sdk/lib/vs2017/x64/release
        ../../Tools/Windows/GLFW3/lib-vc2015
    )

    set(LINK_LIBS ${BASE_LINK_LIBS})

elseif(MINGW)
    message(STATUS "MINGW : ON")
    
    link_directories(
        ../../Tools/Windows/GLFW3/lib-vc2015
    )

    add_definitions(-DUSE_MINGW)

    set(LINK_LIBS ${BASE_LINK_LIBS} pthread)

endif()