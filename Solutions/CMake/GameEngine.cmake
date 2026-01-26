if(BUILD_GAME_ENGINE)
    if(MSVC)
        set(ASSIMP_BUILD_DLL OFF CACHE BOOL "" FORCE)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
        set(USE_MSVC_RUNTIME_LIBRARY_DLL OFF CACHE BOOL "" FORCE)
        set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
        set(BULLET2_MULTITHREADING OFF CACHE BOOL "" FORCE)
    else()
        set(ASSIMP_BUILD_DLL ON CACHE BOOL "" FORCE)
    endif()

    if(WIN32)
        set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "" FORCE)
    else()
        set(ASSIMP_BUILD_ZLIB OFF CACHE BOOL "" FORCE)
    endif()

    set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
    set(BUILD_OPENGL3_DEMOS OFF CACHE BOOL "" FORCE)
    set(USE_GRAPHICAL_BENCHMARK OFF CACHE BOOL "" FORCE)
    set(USE_GLUT OFF CACHE BOOL "" FORCE)
    set(BUILD_CLSOCKET OFF CACHE BOOL "" FORCE)
    set(BUILD_EGL OFF CACHE BOOL "" FORCE)
    set(BUILD_ENET OFF CACHE BOOL "" FORCE)
    set(BUILD_EXTRAS OFF CACHE BOOL "" FORCE)


    if(MSVC)
        
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=maybe-uninitialized")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-class-memaccess")
    endif()

    set(BULLET_DLL_DIR "${CMAKE_CURRENT_BINARY_DIR}/lib")
    add_subdirectory(
        ${CMAKE_CURRENT_SOURCE_DIR}/../../Tools/common/bullet
        ${CMAKE_CURRENT_BINARY_DIR}/bullet
    )
    
    set(BULLET_LIBS
        BulletDynamics
        Bullet3Common
        Bullet3Collision
        Bullet3Dynamics
        Bullet3Geometry
        Bullet3OpenCL_clew
        BulletCollision
        BulletInverseDynamics
        BulletSoftBody
        LinearMath
    )


    set(ASSIMP_DLL_DIR "${CMAKE_CURRENT_BINARY_DIR}/assimp/bin")
    add_subdirectory(
        ${CMAKE_CURRENT_SOURCE_DIR}/../../Tools/common/assimp
        ${CMAKE_CURRENT_BINARY_DIR}/assimp
    )

    get_target_property(_deps assimp::assimp INTERFACE_LINK_LIBRARIES)
    message(STATUS "assimp deps: ${_deps}")

    include(${CMAKE_CURRENT_SOURCE_DIR}/Sources/GameEngineSources.cmake)
    if(MSVC)
        add_library(GameEngineLib STATIC ${GameEngineSources})
    else()
        add_library(GameEngineLib SHARED ${GameEngineSources})
    endif()
    

    set_target_properties(GameEngineLib PROPERTIES WIN32_EXECUTABLE OFF)

    target_compile_options(GameEngineLib PRIVATE ${ENGINE_COMPILE_FLAGS})

    target_include_directories(GameEngineLib
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/Sources
    )

    target_link_libraries(GameEngineLib
        PUBLIC
            assimp::assimp
            ${BULLET_LIBS}
            OpenGLApiLib
            GraphicsApiLib
            InputLib
            UtilsNetworkLib
            CommonLib
            UtilsLib
            ${LINK_LIBS}
    )

    if(WIN32)
        target_link_libraries(GameEngineLib
            PRIVATE
                dbghelp
        )
        copy_dll_to_build_dir(GameEngineLib)
        copy_all_dlls_to_target("${BULLET_DLL_DIR}" GameEngineLib)
        copy_all_dlls_to_target("${ASSIMP_DLL_DIR}" GameEngineLib)
    endif()

endif()
