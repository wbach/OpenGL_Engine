set(GameEngineSources
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Shaders/ShaderProgram.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Physics/Bullet/Converter.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Physics/Bullet/BulletAdapter.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Physics/Bullet/DebugDrawer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Physics/Bach/BachPhysicsAdapter.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/Console/Console.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/Painter/TerrainHeightPainter.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/Painter/TerrainPointGetter.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/Painter/Painter.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/Painter/TerrainHeightGenerator.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/Painter/Brushes/HeightBrushType.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/Painter/Brushes/TextureBrushType.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/Painter/Brushes/Circle/CircleTextureBrushes/CircleLinearTextureBrush.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/Painter/Brushes/Circle/CircleBrushBase.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/Painter/Brushes/Circle/CircleHeightBrushes/CircleLinearHeightBrush.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/Painter/Brushes/Circle/CircleHeightBrushes/CircleConstantHeightBrush.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/Painter/Brushes/Circle/CircleHeightBrushes/CircleHeightBrush.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/Painter/Brushes/Circle/CircleHeightBrushes/CircleGaussianHeightBrush.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/Painter/Brushes/Circle/CircleHeightBrushes/CircleConstantRelativeBrush.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/Painter/Brushes/Circle/CircleHeightBrushes/CircleAverageHeightBrush.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/Painter/PaintType.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/Painter/TerrainTexturePainter.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/Painter/StepInterpolation.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/Painter/PlantPainter.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/EditorInterface/NetworkEditorInterface.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/EditorInterface/CameraEditor.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/EditorInterface/Messages/SceneFileMsgXmlDeserializer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/EditorInterface/Messages/ComponentDataMessageXmlDeserializer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/EditorInterface/Messages/ExitRespXmlSerializer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/EditorInterface/Messages/SceneStartedNotifMsgXmlSerializer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/EditorInterface/Messages/AvailableComponentMsgIndXmlDeserializer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/EditorInterface/Messages/CameraMsgXmlDeserializer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/EditorInterface/Messages/ReloadSceneXmlSerializer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/EditorInterface/Messages/GameObjectRenamedXmlSerializer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/EditorInterface/Messages/AvailableComponentMsgIndXmlSerializer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/EditorInterface/Messages/CameraMsgXmlSerializer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/EditorInterface/Messages/NewLightXmlSerializer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/EditorInterface/Messages/NewGameObjectIndXmlDeserializer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/EditorInterface/Messages/GameObjectDeletedXmlSerializer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/EditorInterface/Messages/TransformXmlSerializer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/EditorInterface/Messages/GameObjectRenamedXmlDeserializer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/EditorInterface/Messages/SceneFileMsgXmlSerializer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/EditorInterface/Messages/LightDeletedXmlDeserializer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/EditorInterface/Messages/NewComponentMsgIndXmlSerializer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/EditorInterface/Messages/ReloadSceneXmlDeserializer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/EditorInterface/Messages/NewGameObjectIndXmlSerializer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/EditorInterface/Messages/RemoveGameObjectIndXmlDeserializer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/EditorInterface/Messages/SelectedObjectChangedXmlDeserializer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/EditorInterface/Messages/PlantBrushEnabledXmlSerializer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/EditorInterface/Messages/LightDeletedXmlSerializer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/EditorInterface/Messages/NewComponentMsgIndXmlDeserializer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/EditorInterface/Messages/ComponentDataMessageXmlSerializer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/EditorInterface/Messages/ParamXmlSerializer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/EditorInterface/Messages/MessageTypes.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/EditorInterface/Messages/RemoveComponentMsgIndXmlDeserializer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/EditorInterface/Messages/RemoveComponentMsgIndXmlSerializer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/EditorInterface/Messages/SceneStartedNotifMsgXmlDeserializer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/EditorInterface/Messages/XmlMessageConverter.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/EditorInterface/Messages/GameObjectDeletedXmlDeserializer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/EditorInterface/Messages/PlantBrushEnabledXmlDeserializer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/EditorInterface/Messages/TransformXmlDeserializer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/EditorInterface/Messages/ComponentDeletedXmlSerializer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/EditorInterface/Messages/NewLightXmlDeserializer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/EditorInterface/Messages/ExitRespXmlDeserializer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/EditorInterface/Messages/TerrainPainterEnabledXmlSerializer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/EditorInterface/Messages/SelectedObjectChangedXmlSerializer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/EditorInterface/Messages/ParamXmlDeserializer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/EditorInterface/Messages/SceneStopedNotifMsgXmlSerializer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/EditorInterface/Messages/ComponentDeletedXmlDeserializer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/EditorInterface/Messages/RemoveGameObjectIndXmlSerializer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/EditorInterface/Messages/TerrainPainterEnabledXmlDeserializer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/EditorInterface/Messages/SceneStopedNotifMsgXmlDeserializer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/Common/MouseUtils.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/MousePicker/DragObject.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/DebugTools/MousePicker/MousePicker.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Display/DisplayManager.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Controllers/CharacterController/Attack/AttackState.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Controllers/CharacterController/AimController.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Controllers/CharacterController/CharacterController.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Controllers/CharacterController/AnimationClipNames.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Controllers/CharacterController/States/DrawArrowWalkState.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Controllers/CharacterController/States/DrawArrowRotateState.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Controllers/CharacterController/States/RecoilWalkAndRotateState.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Controllers/CharacterController/States/ArmedChangeStateBase.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Controllers/CharacterController/States/DeathState.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Controllers/CharacterController/States/DrawArrowState.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Controllers/CharacterController/States/DisarmedRunState.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Controllers/CharacterController/States/AimWalkAndRotateState.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Controllers/CharacterController/States/ArmedRotateState.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Controllers/CharacterController/States/ArmedIdleState.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Controllers/CharacterController/States/AimState.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Controllers/CharacterController/States/MoveStateBase.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Controllers/CharacterController/States/IdleStateBase.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Controllers/CharacterController/States/RecoilState.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Controllers/CharacterController/States/RecoilRunState.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Controllers/CharacterController/States/RunAndRotateArmedChangeState.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Controllers/CharacterController/States/RotateStateBase.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Controllers/CharacterController/States/AimWalkState.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Controllers/CharacterController/States/MoveAndRotateStateBase.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Controllers/CharacterController/States/DrawArrowRunState.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Controllers/CharacterController/States/RecoilRotateState.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Controllers/CharacterController/States/AimRunAndRotateState.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Controllers/CharacterController/States/JumpStateBase.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Controllers/CharacterController/States/RotateArmedChangeState.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Controllers/CharacterController/States/WalkArmedChangeState.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Controllers/CharacterController/States/RecoilRunAndRotateState.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Controllers/CharacterController/States/RecoilWalkState.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Controllers/CharacterController/States/ArmedWalkAndRotateState.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Controllers/CharacterController/States/ArmedRunState.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Controllers/CharacterController/States/RunArmedChangeState.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Controllers/CharacterController/States/DrawArrowRunAndRotateState.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Controllers/CharacterController/States/DrawArrowStateBase.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Controllers/CharacterController/States/RecoilStateBase.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Controllers/CharacterController/States/DisarmedIdleState.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Controllers/CharacterController/States/WalkAndRotateArmedChangeState.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Controllers/CharacterController/States/AimRunState.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Controllers/CharacterController/States/DrawArrowWalkAndRotateState.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Controllers/CharacterController/States/AimRotateState.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Controllers/CharacterController/States/AimStateBase.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Controllers/CharacterController/States/IdleArmedChangeState.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Controllers/EnemyController.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Input/PlayerInputController.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Physics/MeshShape.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Physics/CollisionShape.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Physics/Rigidbody.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Physics/CapsuleShape.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Physics/Terrain/TerrainHeightGetter.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Physics/Terrain/TerrainShape.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Physics/BoxShape.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Physics/SphereShape.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Characters/Bar.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Characters/Enemy.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Characters/Player.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Camera/ThridPersonCamera/ThridPersonCameraComponent.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Camera/ThridPersonCamera/Fsm/States/AimState.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Camera/ThridPersonCamera/Fsm/States/RotateableRunState.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Camera/ThridPersonCamera/Fsm/States/TransitionState.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Camera/ThridPersonCamera/Fsm/States/FollowingState.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Renderer/Particles/ParticleEffectComponent.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Renderer/Trees/TreeRendererComponent.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Renderer/Grass/GrassRendererComponent.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Renderer/SkyBox/SkyBoxComponent.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Renderer/Terrain/TerrainMeshUpdater.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Renderer/Terrain/TerrainQuadTree.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Renderer/Terrain/TerrainTessellationRendererComponent.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Renderer/Terrain/TerrainComponentBase.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Renderer/Terrain/TerrainConfiguration.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Renderer/Terrain/TerrainMeshRendererComponent.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Renderer/Terrain/TerrainTexturesTypes.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Renderer/Terrain/TerrainHeightTools.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Renderer/Terrain/TerrainNode.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Renderer/Entity/PreviewComponent.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Renderer/Entity/RendererComponent.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Renderer/Water/WaterRendererComponent.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Renderer/Skydome/SkydomeComponent.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/BaseComponent.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/CommonReadDef.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/RegisterReadFunctionForDefaultEngineComponents.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Animation/AnimationTransition.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Animation/Common.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Animation/AnimationTransitionMixedToSingle.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Animation/AnimationStateBase.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Animation/EmptyState.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Animation/Animator.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Animation/JointPoseUpdater.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Animation/PlayAnimation.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Animation/StateMachine.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Animation/AnimationTransitionToMixed.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Animation/JointData.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/Animation/PlayMixedAnimation.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/ComponentsReadFunctions.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/ComponentController.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Components/ComponentFactory.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Resources/Models/Model.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Resources/Models/Bone.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Resources/Models/BoundingBox.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Resources/Models/ModelWrapper.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Resources/Models/WBLoader/Collada/Collada.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Resources/Models/WBLoader/Collada/MergeTool/MergeTool.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Resources/Models/WBLoader/Collada/Xml/LibraryImagesReader.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Resources/Models/WBLoader/Collada/Xml/GeometryReader.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Resources/Models/WBLoader/Collada/Xml/LibraryControllersReader.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Resources/Models/WBLoader/Collada/Xml/LibraryAnimationsReader.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Resources/Models/WBLoader/Collada/Xml/LibraryVisualScenesReader.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Resources/Models/WBLoader/Collada/Xml/MainReader.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Resources/Models/WBLoader/Collada/Xml/MaterialReader.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Resources/Models/WBLoader/Collada/Xml/ColladaCommon.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Resources/Models/WBLoader/Collada/Xml/libraryEffectsReader.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Resources/Models/WBLoader/MeshData.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Resources/Models/WBLoader/BEngine/BEngineLoader.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Resources/Models/WBLoader/WaveFront/WaveFrontObj.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Resources/Models/WBLoader/LoaderManager.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Resources/Models/WBLoader/Binary/BinaryWriter.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Resources/Models/WBLoader/Binary/BinaryReader.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Resources/Models/WBLoader/Binary/BinaryLoader.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Resources/Models/WBLoader/Terrain/TerrainMeshLoader.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Resources/Models/WBLoader/Assimp/AssimpLoader.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Resources/Models/WBLoader/AbstractLoader.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Resources/Models/Mesh.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Resources/GpuObject.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Resources/ProgressiveMesh.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Resources/File.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Resources/ResourceManager.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Resources/Textures/GeneralTexture.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Resources/Textures/HeightMap.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Resources/Textures/CubeMapTexture.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Resources/Textures/Texture.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Resources/GpuResourceLoader.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Resources/ResourceUtils.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Resources/BufferObject.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Resources/TextureLoader.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Resources/textureloadertests.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Time/DayNightCycle.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Time/GameTime.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Camera/Camera.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Camera/ThridPersonCamera.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Camera/CameraWrapper.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Camera/FirstPersonCamera.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Camera/Frustrum.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Scene/XmlSceneStorage.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Scene/SceneDef.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Scene/SceneWriter.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Scene/SceneWrapper.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Scene/SceneReader.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Scene/Scene.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Scene/SceneFactoryBase.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Scene/SceneManager.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Scene/SceneLoader.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Engine/Configuration.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Engine/IntroRenderer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Engine/ConfigurationExplorer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Engine/EngineContext.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Engine/ConfigurationReader.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Engine/ConfigurationWriter.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Engine/ConfigurationParams/RendererParams/ParticlesParams/ParticlesParams.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Engine/ConfigurationParams/RendererParams/ShadowsParams/ShadowsParams.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Engine/ConfigurationParams/RendererParams/TerrainParam/TerrainParam.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Engine/ConfigurationParams/RendererParams/TerrainParam/TerrainType.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Engine/ConfigurationParams/RendererParams/RendererParams.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Engine/ConfigurationParams/RendererParams/WaterParams/WaterParams.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Engine/ConfigurationParams/RendererParams/WaterParams/WaterType.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Engine/ConfigurationParams/RendererParams/FloraParams/FloraParams.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Engine/ConfigurationParams/RendererParams/TexturesParams/TexturesParams.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Engine/ConfigurationParams/WindowParams/WindowParams.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Engine/ConfigurationParams/SoundParams/SoundParams.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Engine/ConfigurationParams/ParamToString.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Engine/ConfigurationParams/PramsConstants.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Engine/EngineDef.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Engine/Engine.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/BaseRenderer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/LoadingScreenRenderer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/RenderersManager.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/StaticRayTracer/Camera.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/StaticRayTracer/Ray.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/StaticRayTracer/StepPooler.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/StaticRayTracer/Objects/Sphere.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/StaticRayTracer/Objects/Triangle.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/StaticRayTracer/Objects/BaseObject.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/StaticRayTracer/Objects/Mesh.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/StaticRayTracer/StaticRayTracer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/StaticRayTracer/Intersection.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/IRenderer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/DefferedRenderer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/TransformDataEvent.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/BufferDataUpdater.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/GUI/Text/GuiTextElement.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/GUI/Text/FontManager.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/GUI/TextInput.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/GUI/Button/GuiButton.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/GUI/Layer/GuiLayer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/GUI/GuiElementWriter.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/GUI/Layout/VerticalLayout.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/GUI/Layout/HorizontalLayout.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/GUI/Layout/Layout.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/GUI/GuiElement.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/GUI/GuiElementFactory.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/GUI/GuiEngineContextManger.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/GUI/GuiRenderer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/GUI/TreeView/TreeView.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/GUI/GuiElementsDef.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/GUI/Window/GuiWindowStyle.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/GUI/Window/GuiWindow.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/GUI/GuiElementReader.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/GUI/Menu/Menu.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/GUI/ContextMenu/ContextMenu.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/GUI/GuiRendererElementBase.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/GUI/Texutre/GuiTextureElement.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/GUI/GuiElementTypes.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/GUI/GuiManager.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/GUI/EditText/GuiEditText.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/Objects/Particles/ParticlesRenderer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/Objects/Grass/GrassRenderer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/Objects/Shadows/ShadowMapRenderer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/Objects/Shadows/ShadowBox.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/Objects/Tree/TreeRenderer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/Objects/SkyBox/ConcreteSkyBoxRenderer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/Objects/SkyBox/SkyBoxRenderer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/Objects/Terrain/TerrainRenderer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/Objects/Terrain/Mesh/ConcreteTerrainMeshRenderer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/Objects/Terrain/Mesh/TerrainMeshRenderer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/Objects/Entity/PreviewRenderer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/Objects/Entity/ConcreteEntityRenderer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/Objects/Entity/EntityRenderer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/Objects/Water/WaterRenderer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/Objects/Water/WaterReflectionRefractionRenderer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/Objects/Skydome/SkydomeRenderer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/Projection.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/DebugElements/PhysicsVisualizator.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/DebugElements/DebugRenderer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/Postproccesing/Blur/Blur.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/Postproccesing/DefferedLighting/DefferedLighting.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/Postproccesing/Fxaa/Fxaa.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/Postproccesing/PostprocessingRenderersManager.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/Postproccesing/ColorFliper/ColorFliper.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/Postproccesing/PostprocessingRenderersFactory.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/Postproccesing/Outline/Outline.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Renderers/Postproccesing/SSAO/SSAO.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Lights/Light.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Objects/GameObject.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Animations/Joint.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Animations/AnimationClip.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Animations/AnimationUtils.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Api/Dummy/DummyInputManager.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine/Api/Dummy/DummyGraphicsApi.cpp
)
