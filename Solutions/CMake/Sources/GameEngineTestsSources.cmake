set(GameEngineTestsSources
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngineTests/Tests/MT/GUI/GuiTextTests.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngineTests/Tests/UT/TerrainTransitionTests.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngineTests/Tests/UT/Utils/utilsTests.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngineTests/Tests/UT/Components/AnimatorTests.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngineTests/Tests/UT/Components/Particles/ParticleEffectComponentTests.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngineTests/Tests/UT/Components/BaseComponent.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngineTests/Tests/UT/Components/CharacterController/CharacterControllerMixedTests.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngineTests/Tests/UT/Components/CharacterController/CharacterControllerTests.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngineTests/Tests/UT/Components/CharacterController/States/RecoilWalkStateTests.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngineTests/Tests/UT/Components/CharacterController/States/DisarmedIdleStateTests.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngineTests/Tests/UT/Components/CharacterController/States/DisarmedSprintAndRotateStateTests.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngineTests/Tests/UT/Components/CharacterController/States/AimWalkStateTests.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngineTests/Tests/UT/Components/CharacterController/States/DisarmedRunStateTests.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngineTests/Tests/UT/Components/CharacterController/States/RunAndRotateArmedChangeStateTests.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngineTests/Tests/UT/Components/CharacterController/States/WalkArmedChangeStateTests.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngineTests/Tests/UT/Components/CharacterController/States/AimStateTests.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngineTests/Tests/UT/Components/CharacterController/States/IdleArmedChangeStateTests.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngineTests/Tests/UT/Components/CharacterController/States/DrawArrowStateTests.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngineTests/Tests/UT/Components/CharacterController/States/DisarmedWalkAndRotateStateTests.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngineTests/Tests/UT/Components/CharacterController/States/ArmedSprintStateTests.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngineTests/Tests/UT/Components/CharacterController/States/ArmedRunStateTests.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngineTests/Tests/UT/Components/CharacterController/States/DrawArrowWalkAndRotateStateTests.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngineTests/Tests/UT/Components/CharacterController/States/ArmedRotateStateTests.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngineTests/Tests/UT/Components/CharacterController/States/RotateArmedChangeStateTests.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngineTests/Tests/UT/Components/CharacterController/States/RecoilRotateStateTests.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngineTests/Tests/UT/Components/CharacterController/States/DisarmedWalkStateTests.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngineTests/Tests/UT/Components/CharacterController/States/DisarmedSprintStateTests.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngineTests/Tests/UT/Components/CharacterController/States/AimRotateStateTests.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngineTests/Tests/UT/Components/CharacterController/States/DisarmedRunAndRotateStateTests.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngineTests/Tests/UT/Components/CharacterController/States/ArmedWalkAndRotateStateTests.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngineTests/Tests/UT/Components/CharacterController/States/RecoilWalkAndRotateStateTests.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngineTests/Tests/UT/Components/CharacterController/States/DisarmedAttackStateTests.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngineTests/Tests/UT/Components/CharacterController/States/AimWalkAndRotateStateTests.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngineTests/Tests/UT/Components/CharacterController/States/WalkAndRotateArmedChangeStateTests.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngineTests/Tests/UT/Components/CharacterController/States/DrawArrowWalkStateTests.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngineTests/Tests/UT/Components/CharacterController/States/ArmedWalkStateTests.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngineTests/Tests/UT/Components/CharacterController/States/RunArmedChangeStateTests.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngineTests/Tests/UT/Components/CharacterController/States/ArmedSprintAndRotateStateTests.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngineTests/Tests/UT/Components/CharacterController/States/RecoilStateTests.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngineTests/Tests/UT/Components/CharacterController/States/ArmedRunAndRotateStateTests.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngineTests/Tests/UT/Components/CharacterController/States/ArmedAttackStateTests.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngineTests/Tests/UT/Components/CharacterController/States/ArmedIdleStateTests.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngineTests/Tests/UT/Components/CharacterController/States/DrawArrowRotateStateTests.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngineTests/Tests/UT/Components/CharacterController/States/DisarmedRotateStateTests.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngineTests/Tests/UT/Resources/AssimpLoaderTests.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngineTests/Tests/UT/Resources/ColladaLoaderTests.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngineTests/Tests/UT/Resources/TextureLoaderTests.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngineTests/Tests/UT/Resources/WaveFrontObjLoaderTests.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngineTests/Tests/UT/TerrainToolsTests.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngineTests/Tests/UT/Camera/CameraFrustrumTests.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngineTests/Tests/UT/Scene/Scene.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngineTests/Tests/UT/Renderers/TerrainNormalCalulation.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngineTests/Tests/UT/Renderers/Gui/HorizontalLayoutTests.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngineTests/Tests/UT/Renderers/Gui/VerticalLayoutTests.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngineTests/Tests/UT/Renderers/Gui/GuiElementTests.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngineTests/Tests/UT/Renderers/EntityRendererTest.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngineTests/Tests/UT/TerrainHeightGetterTests.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngineTests/Tests/Mocks/Api/GraphicsApiMock.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngineTests/main.cpp
)
