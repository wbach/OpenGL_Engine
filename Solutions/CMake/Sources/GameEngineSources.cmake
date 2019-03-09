set(GameEngineSources
../../Sources/GameEngine/Animations/AnimationUtils.cpp
../../Sources/GameEngine/Api/Dummy/DummyGraphicsApi.cpp
../../Sources/GameEngine/Api/Dummy/DummyInputManager.cpp
../../Sources/GameEngine/Camera/Camera.cpp
../../Sources/GameEngine/Camera/FirstPersonCamera.cpp
../../Sources/GameEngine/Camera/Frustrum.cpp
../../Sources/GameEngine/Camera/ThridPersonCamera.cpp
../../Sources/GameEngine/Components/Animation/Animator.cpp
../../Sources/GameEngine/Components/BaseComponent.cpp
../../Sources/GameEngine/Components/ComponentController.cpp
../../Sources/GameEngine/Components/ComponentFactory.cpp
../../Sources/GameEngine/Components/Physics/BoxShape.cpp
../../Sources/GameEngine/Components/Physics/CollisionShape.cpp
../../Sources/GameEngine/Components/Physics/DebugDrawner.cpp
../../Sources/GameEngine/Components/Physics/MeshShape.cpp
../../Sources/GameEngine/Components/Physics/Rigidbody.cpp
../../Sources/GameEngine/Components/Physics/SphereShape.cpp
../../Sources/GameEngine/Components/Physics/Terrain/TerrainHeightGetter.cpp
../../Sources/GameEngine/Components/Physics/Terrain/TerrainShape.cpp
../../Sources/GameEngine/Components/Renderer/Entity/RendererComponent.cpp
../../Sources/GameEngine/Components/Renderer/Grass/GrassComponent.cpp
../../Sources/GameEngine/Components/Renderer/Particles/ParticleEffectComponent.cpp
../../Sources/GameEngine/Components/Renderer/SkyBox/SkyBoxComponent.cpp
../../Sources/GameEngine/Components/Renderer/Skydome/SkydomeComponent.cpp
../../Sources/GameEngine/Components/Renderer/Terrain/TerrainConfiguration.cpp
../../Sources/GameEngine/Components/Renderer/Terrain/TerrainMeshRendererComponent.cpp
../../Sources/GameEngine/Components/Renderer/Terrain/TerrainNode.cpp
../../Sources/GameEngine/Components/Renderer/Terrain/TerrainQuadTree.cpp
../../Sources/GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.cpp
../../Sources/GameEngine/Components/Renderer/Trees/TreeRendererComponent.cpp
../../Sources/GameEngine/Display/DisplayManager.cpp
../../Sources/GameEngine/Engine/Configuration.cpp
../../Sources/GameEngine/Engine/ConfigurationReader.cpp
../../Sources/GameEngine/Engine/ConfigurationWriter.cpp
../../Sources/GameEngine/Engine/Engine.cpp
../../Sources/GameEngine/Engine/EngineDef.cpp
../../Sources/GameEngine/Engine/EngineMeasurement.cpp
../../Sources/GameEngine/Engine/IntroRenderer.cpp
../../Sources/GameEngine/Lights/Light.cpp
../../Sources/GameEngine/Objects/GameObject.cpp
../../Sources/GameEngine/Physics/Bach/BachPhysicsAdapter.cpp
../../Sources/GameEngine/Physics/Bullet/BulletAdapter.cpp
../../Sources/GameEngine/Physics/Bullet/DebugDrawer.cpp
../../Sources/GameEngine/Renderers/BaseRenderer.cpp
../../Sources/GameEngine/Renderers/BufferDataUpdater.cpp
../../Sources/GameEngine/Renderers/DefferedRenderer.cpp
../../Sources/GameEngine/Renderers/Framebuffer/DeferedFrameBuffer/DeferedFrameBuffer.cpp
../../Sources/GameEngine/Renderers/Framebuffer/FrameBuffer.cpp
../../Sources/GameEngine/Renderers/Framebuffer/Postprocessing/PostproccesFrameBuffer.cpp
../../Sources/GameEngine/Renderers/GUI/GuiRenderer.cpp
../../Sources/GameEngine/Renderers/GUI/Text/FontShader.cpp
../../Sources/GameEngine/Renderers/GUI/Text/GuiText.cpp
../../Sources/GameEngine/Renderers/GUI/Texutre/GuiTexture.cpp
../../Sources/GameEngine/Renderers/GUI/Texutre/GuiTextureShader.cpp
../../Sources/GameEngine/Renderers/IRenderer.cpp
../../Sources/GameEngine/Renderers/LodingScreenRenderer.cpp
../../Sources/GameEngine/Renderers/Objects/Entity/EntityRenderer.cpp
../../Sources/GameEngine/Renderers/Objects/Entity/Shaders/DefferedEntityShader.cpp
../../Sources/GameEngine/Renderers/Objects/Entity/Shaders/EntityShader.cpp
../../Sources/GameEngine/Renderers/Objects/Grass/GrassRenderer.cpp
../../Sources/GameEngine/Renderers/Objects/Grass/Shaders/GrassShader.cpp
../../Sources/GameEngine/Renderers/Objects/Particles/ParticlesRenderer.cpp
../../Sources/GameEngine/Renderers/Objects/Particles/Shaders/AnimatedParticlesShader.cpp
../../Sources/GameEngine/Renderers/Objects/Particles/Shaders/ParticlesShader.cpp
../../Sources/GameEngine/Renderers/Objects/Plants/PlantsRenderer.cpp
../../Sources/GameEngine/Renderers/Objects/Plants/Shaders/PlantsShader.cpp
../../Sources/GameEngine/Renderers/Objects/Shadows/Shaders/ShadowShader.cpp
../../Sources/GameEngine/Renderers/Objects/Shadows/ShadowBox.cpp
../../Sources/GameEngine/Renderers/Objects/Shadows/ShadowFrameBuffer.cpp
../../Sources/GameEngine/Renderers/Objects/Shadows/ShadowMapRenderer.cpp
../../Sources/GameEngine/Renderers/Objects/SkyBox/Shaders/SkyBoxShader.cpp
../../Sources/GameEngine/Renderers/Objects/SkyBox/SkyBoxRenderer.cpp
../../Sources/GameEngine/Renderers/Objects/Skydome/Shaders/SkydomeShader.cpp
../../Sources/GameEngine/Renderers/Objects/Skydome/SkydomeRenderer.cpp
../../Sources/GameEngine/Renderers/Objects/Terrain/Mesh/Shaders/TerrainMeshShader.cpp
../../Sources/GameEngine/Renderers/Objects/Terrain/Mesh/TerrainMeshRenderer.cpp
../../Sources/GameEngine/Renderers/Objects/Terrain/Shaders/TerrainShader.cpp
../../Sources/GameEngine/Renderers/Objects/Terrain/TerrainRenderer.cpp
../../Sources/GameEngine/Renderers/Objects/Tree/Shaders/TreeShader.cpp
../../Sources/GameEngine/Renderers/Objects/Tree/TreeRenderer.cpp
../../Sources/GameEngine/Renderers/Objects/Water/Shaders/WaterShader.cpp
../../Sources/GameEngine/Renderers/Objects/Water/WaterRenderer.cpp
../../Sources/GameEngine/Renderers/Postproccesing/Blur/Blur.cpp
../../Sources/GameEngine/Renderers/Postproccesing/Blur/Shaders/BlurShader.cpp
../../Sources/GameEngine/Renderers/Postproccesing/ColorFliper/ColorFliper.cpp
../../Sources/GameEngine/Renderers/Postproccesing/ColorFliper/Shaders/ColorFliperShader.cpp
../../Sources/GameEngine/Renderers/Postproccesing/DefferedLighting/DefferedLighting.cpp
../../Sources/GameEngine/Renderers/Postproccesing/DefferedLighting/Shaders/DefferedShader.cpp
../../Sources/GameEngine/Renderers/Postproccesing/PostprocessingRenderer.cpp
../../Sources/GameEngine/Renderers/Postproccesing/PostprocessingRenderersFactory.cpp
../../Sources/GameEngine/Renderers/Postproccesing/PostprocessingRenderersManager.cpp
../../Sources/GameEngine/Renderers/Postproccesing/SSAO/Shaders/SSAOShader.cpp
../../Sources/GameEngine/Renderers/Postproccesing/SSAO/SSAO.cpp
../../Sources/GameEngine/Renderers/Projection.cpp
../../Sources/GameEngine/Renderers/RenderersManager.cpp
../../Sources/GameEngine/Resources/BufferObject.cpp
../../Sources/GameEngine/Resources/GpuResourceLoader.cpp
../../Sources/GameEngine/Resources/Models/Assimp/AssimpModel.cpp
../../Sources/GameEngine/Resources/Models/Bone.cpp
../../Sources/GameEngine/Resources/Models/Mesh.cpp
../../Sources/GameEngine/Resources/Models/Model.cpp
../../Sources/GameEngine/Resources/Models/ModelFactory.cpp
../../Sources/GameEngine/Resources/Models/ModelWrapper.cpp
../../Sources/GameEngine/Resources/Models/WBLoader/AbstractLoader.cpp
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
../../Sources/GameEngine/Resources/Models/WBLoader/LoaderManager.cpp
../../Sources/GameEngine/Resources/Models/WBLoader/MeshData.cpp
../../Sources/GameEngine/Resources/Models/WBLoader/Terrain/TerrainMeshLoader.cpp
../../Sources/GameEngine/Resources/Models/WBLoader/WaveFront/WaveFrontObj.cpp
../../Sources/GameEngine/Resources/ResourceManager.cpp
../../Sources/GameEngine/Resources/TextureLoader.cpp
../../Sources/GameEngine/Resources/textureloadertests.cpp
../../Sources/GameEngine/Resources/Textures/CubeMapTexture.cpp
../../Sources/GameEngine/Resources/Textures/GeneralTexture.cpp
../../Sources/GameEngine/Resources/Textures/HeightMap.cpp
../../Sources/GameEngine/Resources/Textures/MaterialTexture.cpp
../../Sources/GameEngine/Resources/Textures/Texture.cpp
../../Sources/GameEngine/Scene/Scene.cpp
../../Sources/GameEngine/Scene/SceneDef.cpp
../../Sources/GameEngine/Scene/SceneFactoryBase.cpp
../../Sources/GameEngine/Scene/SceneLoader.cpp
../../Sources/GameEngine/Scene/SceneManager.cpp
../../Sources/GameEngine/Scene/SceneReader.cpp
../../Sources/GameEngine/Scene/SceneWrapper.cpp
../../Sources/GameEngine/Scene/SceneWriter.cpp
../../Sources/GameEngine/Shaders/Loading/LoadingShader.cpp
../../Sources/GameEngine/Shaders/ShaderFactory.cpp
../../Sources/GameEngine/Shaders/ShaderProgram.cpp
../../Sources/GameEngine/Time/DayNightCycle.cpp
../../Sources/GameEngine/Time/GameTime.cpp
)
