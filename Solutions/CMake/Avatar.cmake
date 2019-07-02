if(BUILD_AVATAR_GAME)
    option(BUILD_UNIT_TESTS "" OFF)
    set(BulletLinkingLibs
                BulletDynamics
                Bullet3Common
                Bullet2FileLoader
                Bullet3Collision
                Bullet3Dynamics
                Bullet3Geometry
                Bullet3OpenCL_clew
                BulletCollision
                BulletFileLoader
                BulletInverseDynamics
                BulletInverseDynamicsUtils
                BulletRobotics
                BulletSoftBody
                BulletWorldImporter
                BulletXmlWorldImporter
                BussIK
                clsocket
                ConvexDecomposition
                GIMPACTUtils
                HACD
                OpenGLWindow
                BulletExampleBrowserLib
                gwen
                LinearMath
                fbxsdk
          )

    set(BoostLinkingLibs
        boost_system
        boost_filesystem
    )

    include_directories(
    ../../Sources/GameEngine/
    )
    include(./Sources/AvatarSources.cmake)
    add_executable(AvatarGame ${AvatarSources})
    if(BUILD_I386)
        set_target_properties(AvatarGame PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
    endif()
    target_link_libraries(AvatarGame GameEngineLib OpenGLApiLib InputLib UtilsNetworkLib CommonLib UtilsLib ${LinkingLibs} ${BulletLinkingLibs} ${BoostLinkingLibs})
endif()