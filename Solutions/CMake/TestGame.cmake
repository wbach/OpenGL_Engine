if(BUILD_TEST_GAME)
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
          )
    link_directories(${CMAKE_CURRENT_BINARY_DIR}/bullet)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../Tools/common/bullet/ ${CMAKE_CURRENT_BINARY_DIR}/bullet)

    include_directories(
    ../../Sources/GameEngine/
    )
    include(./Sources/TestGameSources.cmake)
    add_executable(TestGlGame ${TestGameSources})
    if(BUILD_I386)
        set_target_properties(TestGlGame PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
    endif()
    target_link_libraries(TestGlGame GameEngineLib OpenGLApiLib InputLib UtilsNetworkLib CommonLib UtilsLib ${LinkingLibs} ${BulletLinkingLibs})
endif()