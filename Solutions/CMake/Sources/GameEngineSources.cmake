set(GameEngineSources
../../Sources/GameEngine/Animations/AnimationClip.cpp
../../Sources/GameEngine/Animations/AnimationUtils.cpp
../../Sources/GameEngine/Animations/Joint.cpp
../../Sources/GameEngine/Api/Dummy/DummyGraphicsApi.cpp
../../Sources/GameEngine/Api/Dummy/DummyInputManager.cpp
../../Sources/GameEngine/Camera/Camera.cpp
../../Sources/GameEngine/Camera/CameraWrapper.cpp
../../Sources/GameEngine/Camera/FirstPersonCamera.cpp
../../Sources/GameEngine/Camera/Frustrum.cpp
../../Sources/GameEngine/Camera/ThridPersonCamera.cpp
../../Sources/GameEngine/Components/Animation/AnimationTransition.cpp
../../Sources/GameEngine/Components/Animation/AnimationTransitionEvent.cpp
../../Sources/GameEngine/Components/Animation/Animator.cpp
../../Sources/GameEngine/Components/Animation/Common.cpp
../../Sources/GameEngine/Components/Animation/EndAnimationTransitionEvent.cpp
../../Sources/GameEngine/Components/Animation/JointData.cpp
../../Sources/GameEngine/Components/Animation/PlayAnimation.cpp
../../Sources/GameEngine/Components/Animation/PlayAnimationEvent.cpp
../../Sources/GameEngine/Components/Animation/PlayMixedAnimation.cpp
../../Sources/GameEngine/Components/Animation/PlayMixedAnimationEvent.cpp
../../Sources/GameEngine/Components/Animation/StateMachine.cpp
../../Sources/GameEngine/Components/BaseComponent.cpp
../../Sources/GameEngine/Components/Camera/ThridPersonCameraComponent.cpp
../../Sources/GameEngine/Components/Characters/Bar.cpp
../../Sources/GameEngine/Components/Characters/Enemy.cpp
../../Sources/GameEngine/Components/Characters/Player.cpp
../../Sources/GameEngine/Components/CommonReadDef.cpp
../../Sources/GameEngine/Components/ComponentController.cpp
../../Sources/GameEngine/Components/ComponentFactory.cpp
../../Sources/GameEngine/Components/ComponentsReadFunctions.cpp
../../Sources/GameEngine/Components/Controllers/CharacterController.cpp
../../Sources/GameEngine/Components/Controllers/EnemyController.cpp
../../Sources/GameEngine/Components/Input/PlayerInputController.cpp
../../Sources/GameEngine/Components/Physics/BoxShape.cpp
../../Sources/GameEngine/Components/Physics/CapsuleShape.cpp
../../Sources/GameEngine/Components/Physics/CollisionShape.cpp
../../Sources/GameEngine/Components/Physics/MeshShape.cpp
../../Sources/GameEngine/Components/Physics/Rigidbody.cpp
../../Sources/GameEngine/Components/Physics/SphereShape.cpp
../../Sources/GameEngine/Components/Physics/Terrain/TerrainHeightGetter.cpp
../../Sources/GameEngine/Components/Physics/Terrain/TerrainShape.cpp
../../Sources/GameEngine/Components/RegisterReadFunctionForDefaultEngineComponents.cpp
../../Sources/GameEngine/Components/Renderer/Entity/PreviewComponent.cpp
../../Sources/GameEngine/Components/Renderer/Entity/RendererComponent.cpp
../../Sources/GameEngine/Components/Renderer/Grass/GrassRendererComponent.cpp
../../Sources/GameEngine/Components/Renderer/Particles/ParticleEffectComponent.cpp
../../Sources/GameEngine/Components/Renderer/SkyBox/SkyBoxComponent.cpp
../../Sources/GameEngine/Components/Renderer/Skydome/SkydomeComponent.cpp
../../Sources/GameEngine/Components/Renderer/Terrain/TerrainComponentBase.cpp
../../Sources/GameEngine/Components/Renderer/Terrain/TerrainConfiguration.cpp
../../Sources/GameEngine/Components/Renderer/Terrain/TerrainHeightTools.cpp
../../Sources/GameEngine/Components/Renderer/Terrain/TerrainMeshRendererComponent.cpp
../../Sources/GameEngine/Components/Renderer/Terrain/TerrainMeshUpdater.cpp
../../Sources/GameEngine/Components/Renderer/Terrain/TerrainNode.cpp
../../Sources/GameEngine/Components/Renderer/Terrain/TerrainQuadTree.cpp
../../Sources/GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.cpp
../../Sources/GameEngine/Components/Renderer/Terrain/TerrainTessellationRendererComponent.cpp
../../Sources/GameEngine/Components/Renderer/Terrain/TerrainTexturesTypes.cpp
../../Sources/GameEngine/Components/Renderer/Trees/TreeRendererComponent.cpp
../../Sources/GameEngine/Components/Renderer/Water/WaterRendererComponent.cpp
../../Sources/GameEngine/DebugTools/Common/MouseUtils.cpp
../../Sources/GameEngine/DebugTools/Console/Console.cpp
../../Sources/GameEngine/DebugTools/EditorInterface/CameraEditor.cpp
../../Sources/GameEngine/DebugTools/EditorInterface/Messages/AvailableComponentMsgIndXmlDeserializer.cpp
../../Sources/GameEngine/DebugTools/EditorInterface/Messages/AvailableComponentMsgIndXmlSerializer.cpp
../../Sources/GameEngine/DebugTools/EditorInterface/Messages/CameraMsgXmlDeserializer.cpp
../../Sources/GameEngine/DebugTools/EditorInterface/Messages/CameraMsgXmlSerializer.cpp
../../Sources/GameEngine/DebugTools/EditorInterface/Messages/ComponentDataMessageXmlDeserializer.cpp
../../Sources/GameEngine/DebugTools/EditorInterface/Messages/ComponentDataMessageXmlSerializer.cpp
../../Sources/GameEngine/DebugTools/EditorInterface/Messages/ComponentDeletedXmlDeserializer.cpp
../../Sources/GameEngine/DebugTools/EditorInterface/Messages/ComponentDeletedXmlSerializer.cpp
../../Sources/GameEngine/DebugTools/EditorInterface/Messages/ExitRespXmlDeserializer.cpp
../../Sources/GameEngine/DebugTools/EditorInterface/Messages/ExitRespXmlSerializer.cpp
../../Sources/GameEngine/DebugTools/EditorInterface/Messages/GameObjectDeletedXmlDeserializer.cpp
../../Sources/GameEngine/DebugTools/EditorInterface/Messages/GameObjectDeletedXmlSerializer.cpp
../../Sources/GameEngine/DebugTools/EditorInterface/Messages/GameObjectRenamedXmlDeserializer.cpp
../../Sources/GameEngine/DebugTools/EditorInterface/Messages/GameObjectRenamedXmlSerializer.cpp
../../Sources/GameEngine/DebugTools/EditorInterface/Messages/LightDeletedXmlDeserializer.cpp
../../Sources/GameEngine/DebugTools/EditorInterface/Messages/LightDeletedXmlSerializer.cpp
../../Sources/GameEngine/DebugTools/EditorInterface/Messages/MessageTypes.cpp
../../Sources/GameEngine/DebugTools/EditorInterface/Messages/NewComponentMsgIndXmlDeserializer.cpp
../../Sources/GameEngine/DebugTools/EditorInterface/Messages/NewComponentMsgIndXmlSerializer.cpp
../../Sources/GameEngine/DebugTools/EditorInterface/Messages/NewGameObjectIndXmlDeserializer.cpp
../../Sources/GameEngine/DebugTools/EditorInterface/Messages/NewGameObjectIndXmlSerializer.cpp
../../Sources/GameEngine/DebugTools/EditorInterface/Messages/NewLightXmlDeserializer.cpp
../../Sources/GameEngine/DebugTools/EditorInterface/Messages/NewLightXmlSerializer.cpp
../../Sources/GameEngine/DebugTools/EditorInterface/Messages/ParamXmlDeserializer.cpp
../../Sources/GameEngine/DebugTools/EditorInterface/Messages/ParamXmlSerializer.cpp
../../Sources/GameEngine/DebugTools/EditorInterface/Messages/PlantBrushEnabledXmlDeserializer.cpp
../../Sources/GameEngine/DebugTools/EditorInterface/Messages/PlantBrushEnabledXmlSerializer.cpp
../../Sources/GameEngine/DebugTools/EditorInterface/Messages/ReloadSceneXmlDeserializer.cpp
../../Sources/GameEngine/DebugTools/EditorInterface/Messages/ReloadSceneXmlSerializer.cpp
../../Sources/GameEngine/DebugTools/EditorInterface/Messages/RemoveComponentMsgIndXmlDeserializer.cpp
../../Sources/GameEngine/DebugTools/EditorInterface/Messages/RemoveComponentMsgIndXmlSerializer.cpp
../../Sources/GameEngine/DebugTools/EditorInterface/Messages/RemoveGameObjectIndXmlDeserializer.cpp
../../Sources/GameEngine/DebugTools/EditorInterface/Messages/RemoveGameObjectIndXmlSerializer.cpp
../../Sources/GameEngine/DebugTools/EditorInterface/Messages/SceneFileMsgXmlDeserializer.cpp
../../Sources/GameEngine/DebugTools/EditorInterface/Messages/SceneFileMsgXmlSerializer.cpp
../../Sources/GameEngine/DebugTools/EditorInterface/Messages/SceneStartedNotifMsgXmlDeserializer.cpp
../../Sources/GameEngine/DebugTools/EditorInterface/Messages/SceneStartedNotifMsgXmlSerializer.cpp
../../Sources/GameEngine/DebugTools/EditorInterface/Messages/SceneStopedNotifMsgXmlDeserializer.cpp
../../Sources/GameEngine/DebugTools/EditorInterface/Messages/SceneStopedNotifMsgXmlSerializer.cpp
../../Sources/GameEngine/DebugTools/EditorInterface/Messages/SelectedObjectChangedXmlDeserializer.cpp
../../Sources/GameEngine/DebugTools/EditorInterface/Messages/SelectedObjectChangedXmlSerializer.cpp
../../Sources/GameEngine/DebugTools/EditorInterface/Messages/TerrainPainterEnabledXmlDeserializer.cpp
../../Sources/GameEngine/DebugTools/EditorInterface/Messages/TerrainPainterEnabledXmlSerializer.cpp
../../Sources/GameEngine/DebugTools/EditorInterface/Messages/TransformXmlDeserializer.cpp
../../Sources/GameEngine/DebugTools/EditorInterface/Messages/TransformXmlSerializer.cpp
../../Sources/GameEngine/DebugTools/EditorInterface/Messages/XmlMessageConverter.cpp
../../Sources/GameEngine/DebugTools/EditorInterface/NetworkEditorInterface.cpp
../../Sources/GameEngine/DebugTools/MousePicker/DragObject.cpp
../../Sources/GameEngine/DebugTools/MousePicker/MousePicker.cpp
../../Sources/GameEngine/DebugTools/Painter/Brushes/Circle/CircleBrushBase.cpp
../../Sources/GameEngine/DebugTools/Painter/Brushes/Circle/CircleHeightBrushes/CircleAverageHeightBrush.cpp
../../Sources/GameEngine/DebugTools/Painter/Brushes/Circle/CircleHeightBrushes/CircleConstantHeightBrush.cpp
../../Sources/GameEngine/DebugTools/Painter/Brushes/Circle/CircleHeightBrushes/CircleConstantRelativeBrush.cpp
../../Sources/GameEngine/DebugTools/Painter/Brushes/Circle/CircleHeightBrushes/CircleGaussianHeightBrush.cpp
../../Sources/GameEngine/DebugTools/Painter/Brushes/Circle/CircleHeightBrushes/CircleHeightBrush.cpp
../../Sources/GameEngine/DebugTools/Painter/Brushes/Circle/CircleHeightBrushes/CircleLinearHeightBrush.cpp
../../Sources/GameEngine/DebugTools/Painter/Brushes/Circle/CircleTextureBrushes/CircleLinearTextureBrush.cpp
../../Sources/GameEngine/DebugTools/Painter/Brushes/HeightBrushType.cpp
../../Sources/GameEngine/DebugTools/Painter/Brushes/TextureBrushType.cpp
../../Sources/GameEngine/DebugTools/Painter/Painter.cpp
../../Sources/GameEngine/DebugTools/Painter/PaintType.cpp
../../Sources/GameEngine/DebugTools/Painter/PlantPainter.cpp
../../Sources/GameEngine/DebugTools/Painter/StepInterpolation.cpp
../../Sources/GameEngine/DebugTools/Painter/TerrainHeightGenerator.cpp
../../Sources/GameEngine/DebugTools/Painter/TerrainHeightPainter.cpp
../../Sources/GameEngine/DebugTools/Painter/TerrainPointGetter.cpp
../../Sources/GameEngine/DebugTools/Painter/TerrainTexturePainter.cpp
../../Sources/GameEngine/Display/DisplayManager.cpp
../../Sources/GameEngine/Engine/Configuration.cpp
../../Sources/GameEngine/Engine/ConfigurationExplorer.cpp
../../Sources/GameEngine/Engine/ConfigurationParams/ParamToString.cpp
../../Sources/GameEngine/Engine/ConfigurationParams/PramsConstants.cpp
../../Sources/GameEngine/Engine/ConfigurationParams/RendererParams/FloraParams/FloraParams.cpp
../../Sources/GameEngine/Engine/ConfigurationParams/RendererParams/ParticlesParams/ParticlesParams.cpp
../../Sources/GameEngine/Engine/ConfigurationParams/RendererParams/RendererParams.cpp
../../Sources/GameEngine/Engine/ConfigurationParams/RendererParams/ShadowsParams/ShadowsParams.cpp
../../Sources/GameEngine/Engine/ConfigurationParams/RendererParams/TerrainParam/TerrainParam.cpp
../../Sources/GameEngine/Engine/ConfigurationParams/RendererParams/TerrainParam/TerrainType.cpp
../../Sources/GameEngine/Engine/ConfigurationParams/RendererParams/TexturesParams/TexturesParams.cpp
../../Sources/GameEngine/Engine/ConfigurationParams/RendererParams/WaterParams/WaterParams.cpp
../../Sources/GameEngine/Engine/ConfigurationParams/RendererParams/WaterParams/WaterType.cpp
../../Sources/GameEngine/Engine/ConfigurationParams/SoundParams/SoundParams.cpp
../../Sources/GameEngine/Engine/ConfigurationParams/WindowParams/WindowParams.cpp
../../Sources/GameEngine/Engine/ConfigurationReader.cpp
../../Sources/GameEngine/Engine/ConfigurationWriter.cpp
../../Sources/GameEngine/Engine/Engine.cpp
../../Sources/GameEngine/Engine/EngineContext.cpp
../../Sources/GameEngine/Engine/EngineDef.cpp
../../Sources/GameEngine/Engine/IntroRenderer.cpp
../../Sources/GameEngine/Lights/Light.cpp
../../Sources/GameEngine/Objects/GameObject.cpp
../../Sources/GameEngine/Physics/Bach/BachPhysicsAdapter.cpp
../../Sources/GameEngine/Physics/Bullet/BulletAdapter.cpp
../../Sources/GameEngine/Physics/Bullet/Converter.cpp
../../Sources/GameEngine/Physics/Bullet/DebugDrawer.cpp
../../Sources/GameEngine/Renderers/BaseRenderer.cpp
../../Sources/GameEngine/Renderers/BufferDataUpdater.cpp
../../Sources/GameEngine/Renderers/DebugElements/DebugRenderer.cpp
../../Sources/GameEngine/Renderers/DebugElements/PhysicsVisualizator.cpp
../../Sources/GameEngine/Renderers/DefferedRenderer.cpp
../../Sources/GameEngine/Renderers/GUI/Button/GuiButton.cpp
../../Sources/GameEngine/Renderers/GUI/ContextMenu/ContextMenu.cpp
../../Sources/GameEngine/Renderers/GUI/EditText/GuiEditText.cpp
../../Sources/GameEngine/Renderers/GUI/GuiElement.cpp
../../Sources/GameEngine/Renderers/GUI/GuiElementFactory.cpp
../../Sources/GameEngine/Renderers/GUI/GuiElementReader.cpp
../../Sources/GameEngine/Renderers/GUI/GuiElementsDef.cpp
../../Sources/GameEngine/Renderers/GUI/GuiElementTypes.cpp
../../Sources/GameEngine/Renderers/GUI/GuiElementWriter.cpp
../../Sources/GameEngine/Renderers/GUI/GuiEngineContextManger.cpp
../../Sources/GameEngine/Renderers/GUI/GuiManager.cpp
../../Sources/GameEngine/Renderers/GUI/GuiRenderer.cpp
../../Sources/GameEngine/Renderers/GUI/GuiRendererElementBase.cpp
../../Sources/GameEngine/Renderers/GUI/Layer/GuiLayer.cpp
../../Sources/GameEngine/Renderers/GUI/Layout/HorizontalLayout.cpp
../../Sources/GameEngine/Renderers/GUI/Layout/Layout.cpp
../../Sources/GameEngine/Renderers/GUI/Layout/VerticalLayout.cpp
../../Sources/GameEngine/Renderers/GUI/Text/FontManager.cpp
../../Sources/GameEngine/Renderers/GUI/Text/GuiTextElement.cpp
../../Sources/GameEngine/Renderers/GUI/TextInput.cpp
../../Sources/GameEngine/Renderers/GUI/Texutre/GuiTextureElement.cpp
../../Sources/GameEngine/Renderers/GUI/TreeView/TreeView.cpp
../../Sources/GameEngine/Renderers/GUI/Window/GuiWindow.cpp
../../Sources/GameEngine/Renderers/GUI/Window/GuiWindowStyle.cpp
../../Sources/GameEngine/Renderers/IRenderer.cpp
../../Sources/GameEngine/Renderers/LoadingScreenRenderer.cpp
../../Sources/GameEngine/Renderers/Objects/Entity/ConcreteEntityRenderer.cpp
../../Sources/GameEngine/Renderers/Objects/Entity/EntityRenderer.cpp
../../Sources/GameEngine/Renderers/Objects/Entity/PreviewRenderer.cpp
../../Sources/GameEngine/Renderers/Objects/Grass/GrassRenderer.cpp
../../Sources/GameEngine/Renderers/Objects/Particles/ParticlesRenderer.cpp
../../Sources/GameEngine/Renderers/Objects/Shadows/ShadowBox.cpp
../../Sources/GameEngine/Renderers/Objects/Shadows/ShadowMapRenderer.cpp
../../Sources/GameEngine/Renderers/Objects/SkyBox/ConcreteSkyBoxRenderer.cpp
../../Sources/GameEngine/Renderers/Objects/SkyBox/SkyBoxRenderer.cpp
../../Sources/GameEngine/Renderers/Objects/Skydome/SkydomeRenderer.cpp
../../Sources/GameEngine/Renderers/Objects/Terrain/Mesh/ConcreteTerrainMeshRenderer.cpp
../../Sources/GameEngine/Renderers/Objects/Terrain/Mesh/TerrainMeshRenderer.cpp
../../Sources/GameEngine/Renderers/Objects/Terrain/TerrainRenderer.cpp
../../Sources/GameEngine/Renderers/Objects/Tree/TreeRenderer.cpp
../../Sources/GameEngine/Renderers/Objects/Water/WaterReflectionRefractionRenderer.cpp
../../Sources/GameEngine/Renderers/Objects/Water/WaterRenderer.cpp
../../Sources/GameEngine/Renderers/Postproccesing/Blur/Blur.cpp
../../Sources/GameEngine/Renderers/Postproccesing/ColorFliper/ColorFliper.cpp
../../Sources/GameEngine/Renderers/Postproccesing/DefferedLighting/DefferedLighting.cpp
../../Sources/GameEngine/Renderers/Postproccesing/Fxaa/Fxaa.cpp
../../Sources/GameEngine/Renderers/Postproccesing/PostprocessingRenderersFactory.cpp
../../Sources/GameEngine/Renderers/Postproccesing/PostprocessingRenderersManager.cpp
../../Sources/GameEngine/Renderers/Postproccesing/SSAO/SSAO.cpp
../../Sources/GameEngine/Renderers/Projection.cpp
../../Sources/GameEngine/Renderers/RenderersManager.cpp
../../Sources/GameEngine/Renderers/TransformDataEvent.cpp
../../Sources/GameEngine/Resources/BufferObject.cpp
../../Sources/GameEngine/Resources/File.cpp
../../Sources/GameEngine/Resources/GpuObject.cpp
../../Sources/GameEngine/Resources/GpuResourceLoader.cpp
../../Sources/GameEngine/Resources/Models/Bone.cpp
../../Sources/GameEngine/Resources/Models/BoundingBox.cpp
../../Sources/GameEngine/Resources/Models/Mesh.cpp
../../Sources/GameEngine/Resources/Models/Model.cpp
../../Sources/GameEngine/Resources/Models/ModelWrapper.cpp
../../Sources/GameEngine/Resources/Models/WBLoader/AbstractLoader.cpp
../../Sources/GameEngine/Resources/Models/WBLoader/Assimp/AssimpLoader.cpp
../../Sources/GameEngine/Resources/Models/WBLoader/BEngine/BEngineLoader.cpp
../../Sources/GameEngine/Resources/Models/WBLoader/Binary/BinaryLoader.cpp
../../Sources/GameEngine/Resources/Models/WBLoader/Binary/BinaryReader.cpp
../../Sources/GameEngine/Resources/Models/WBLoader/Binary/BinaryWriter.cpp
../../Sources/GameEngine/Resources/Models/WBLoader/Collada/Collada.cpp
../../Sources/GameEngine/Resources/Models/WBLoader/Collada/MergeTool/MergeTool.cpp
../../Sources/GameEngine/Resources/Models/WBLoader/Collada/Xml/ColladaCommon.cpp
../../Sources/GameEngine/Resources/Models/WBLoader/Collada/Xml/GeometryReader.cpp
../../Sources/GameEngine/Resources/Models/WBLoader/Collada/Xml/LibraryAnimationsReader.cpp
../../Sources/GameEngine/Resources/Models/WBLoader/Collada/Xml/LibraryControllersReader.cpp
../../Sources/GameEngine/Resources/Models/WBLoader/Collada/Xml/libraryEffectsReader.cpp
../../Sources/GameEngine/Resources/Models/WBLoader/Collada/Xml/LibraryImagesReader.cpp
../../Sources/GameEngine/Resources/Models/WBLoader/Collada/Xml/LibraryVisualScenesReader.cpp
../../Sources/GameEngine/Resources/Models/WBLoader/Collada/Xml/MainReader.cpp
../../Sources/GameEngine/Resources/Models/WBLoader/Collada/Xml/MaterialReader.cpp
../../Sources/GameEngine/Resources/Models/WBLoader/Fbx/FbxLoader.cpp
../../Sources/GameEngine/Resources/Models/WBLoader/LoaderManager.cpp
../../Sources/GameEngine/Resources/Models/WBLoader/MeshData.cpp
../../Sources/GameEngine/Resources/Models/WBLoader/Terrain/TerrainMeshLoader.cpp
../../Sources/GameEngine/Resources/Models/WBLoader/WaveFront/WaveFrontObj.cpp
../../Sources/GameEngine/Resources/ProgressiveMesh.cpp
../../Sources/GameEngine/Resources/ResourceManager.cpp
../../Sources/GameEngine/Resources/ResourceUtils.cpp
../../Sources/GameEngine/Resources/TextureLoader.cpp
../../Sources/GameEngine/Resources/textureloadertests.cpp
../../Sources/GameEngine/Resources/Textures/CubeMapTexture.cpp
../../Sources/GameEngine/Resources/Textures/GeneralTexture.cpp
../../Sources/GameEngine/Resources/Textures/HeightMap.cpp
../../Sources/GameEngine/Resources/Textures/Texture.cpp
../../Sources/GameEngine/Scene/Scene.cpp
../../Sources/GameEngine/Scene/SceneDef.cpp
../../Sources/GameEngine/Scene/SceneFactoryBase.cpp
../../Sources/GameEngine/Scene/SceneLoader.cpp
../../Sources/GameEngine/Scene/SceneManager.cpp
../../Sources/GameEngine/Scene/SceneReader.cpp
../../Sources/GameEngine/Scene/SceneWrapper.cpp
../../Sources/GameEngine/Scene/SceneWriter.cpp
../../Sources/GameEngine/Scene/XmlSceneStorage.cpp
../../Sources/GameEngine/Shaders/ShaderProgram.cpp
../../Sources/GameEngine/Time/DayNightCycle.cpp
../../Sources/GameEngine/Time/GameTime.cpp
)
