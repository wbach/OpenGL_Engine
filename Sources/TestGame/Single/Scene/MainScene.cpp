
#include "MainScene.h"
#include "GLM/GLMUtils.h"
#include "GameEngine/Camera/FirstPersonCamera.h"
#include "GameEngine/Camera/ThridPersonCamera.h"
#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/Physics/BoxShape.h"
#include "GameEngine/Components/Physics/MeshShape.h"
#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/Components/Physics/SphereShape.h"
#include "GameEngine/Components/Physics/Terrain/TerrainShape.h"
#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"
#include "GameEngine/Components/Renderer/Grass/GrassRendererComponent.h"
#include "GameEngine/Components/Renderer/Particles/ParticleEffectComponent.h"
#include "GameEngine/Components/Renderer/SkyBox/SkyBoxComponent.h"
#include "GameEngine/Components/Renderer/Skydome/SkydomeComponent.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainMeshRendererComponent.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Components/Renderer/Trees/TreeRendererComponent.h"
#include "GameEngine/Components/Renderer/Water/WaterRendererComponent.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Engine/Engine.h"
#include "GameEngine/Lights/Light.h"
#include "GameEngine/Renderers/GUI/GuiRenderer.h"
#include "GameEngine/Resources/ResourceUtils.h"
#include "GameEngine/Resources/Textures/HeightMap.h"
#include "GraphicsApi/BlendFunctionsTypes.h"
#include "Renderers/GUI/Button/GuiButton.h"
#include "Renderers/GUI/Texutre/GuiTextureElement.h"
#include "Renderers/GUI/Window/GuiWindow.h"
#include "SingleTon.h"
#include "Thread.hpp"
#include <Utils/FileSystem/FileSystemUtils.hpp>

using namespace GameEngine;

MainScene::MainScene()
    : Scene("MainScene")
{
}

MainScene::~MainScene()
{
    DEBUG_LOG(__FUNCTION__);
}

float Random()
{
    return static_cast<float>(rand() % 100000) / 100000.f;
}

int MainScene::Initialize()
{
    inputManager_->SubscribeOnKeyDown(KeyCodes::ESCAPE, [&]() { addEngineEvent(EngineEvent::QUIT); });
    return 0;
}

int MainScene::Update(float dt)
{
    return 0;
}
void MainScene::KeyOperations()
{
}
