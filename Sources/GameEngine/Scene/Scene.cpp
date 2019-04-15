#include "Scene.hpp"
#include "GameEngine/Camera/Camera.h"
#include "GameEngine/Components/ComponentFactory.h"
#include "GameEngine/Display/DisplayManager.hpp"
#include "GameEngine/Engine/EngineMeasurement.h"
#include "GameEngine/Renderers/GUI/GuiRenderer.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "Input/InputManager.h"
#include "Logger/Log.h"
#include "SceneReader.h"
#include "SceneWriter.h"
#include "Utils/Time/Timer.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Renderers/GUI/Text/GuiTextElement.h"

namespace GameEngine
{
namespace
{
std::mutex cameraSwitchMutex;
}  // namespace

Scene::Scene(const std::string& name)
    : objectCount(0)
    , name(name)
    , inputManager_(nullptr)
    , displayManager_(nullptr)
    , renderersManager_(nullptr)
    , physicsApi_(nullptr)
    , gloabalTime(0.f)
    , directionalLight(vec3(10000, 15000, 10000), vec3(0.8))
    , camera(new BaseCamera(vec3(0, 5, 5), vec3(0, 0, 0)))
    , componentFactory_(nullptr)
    , simulatePhysics_(true)
{
}

Scene::~Scene()
{
    DEBUG_LOG("");

    gameObjects.clear();
    camera.reset();

    if (inputManager_ != nullptr)
    {
        inputManager_->UnsubscribeAll();
    }
}

void Scene::Init()
{
    componentFactory_ = std::make_unique<Components::ComponentFactory>(componentController_, time_, *resourceManager_,
                                                                       *renderersManager_, camera, *physicsApi_);
    Initialize();
    componentController_.OnAwake();
    componentController_.OnStart();
}

void Scene::PostInit()
{
}

void Scene::FullUpdate(float deltaTime)
{
    if (physicsApi_ && simulatePhysics_.load())
    {
        physicsApi_->SetSimulationStep(deltaTime);
        physicsApi_->Simulate();
    }

    if (displayManager_ != nullptr)
    {
        time_.deltaTime = deltaTime;
    }

    Update(deltaTime);
    componentController_.Update();

    if (inputManager_ != nullptr)
    {
        inputManager_->ProcessKeysEvents();
    }
}

void Scene::PostUpdate()
{
    componentController_.PostUpdate();
}

void Scene::CreateResourceManger(IResourceManager* resourceManager)
{
    resourceManager_.reset(resourceManager);
    guiTextFactory_ = std::make_unique<GuiTextFactory>(*resourceManager, EngineConf.renderer.resolution);
}

void Scene::SetRenderersManager(Renderer::RenderersManager* manager)
{
    guiManager_ = std::make_unique<GuiManager>([manager](auto& element)
    {
        manager->GetGuiRenderer().Subscribe(element);
    });

    renderersManager_ = manager;
}

GuiTextElement* Scene::CreateGuiText(const std::string& label, const std::string& font,
    const std::string& str, uint32 size, uint32 outline)
{
    auto text = guiTextFactory_->Create(font, str, size, outline);
    auto result = text.get();
    guiManager_->Add(label, std::move(text));
    return result;
}
GuiTextElement* Scene::GuiText(const std::string& label)
{
    return guiManager_->Get<GuiTextElement>(label);
}
GuiTextureElement* Scene::GuiTexture(const std::string& label)
{
    return guiManager_->Get<GuiTextureElement>(label);
}

std::unique_ptr<GameObject> Scene::CreateGameObject() const
{
    return std::make_unique<GameObject>("gameObject", *componentFactory_);
}

std::unique_ptr<GameObject> Scene::CreateGameObject(const std::string& name) const
{
    return std::make_unique<GameObject>(name, *componentFactory_);
}

void Scene::SetDirectionalLightColor(const vec3& color)
{
    directionalLight.SetColor(color);
}

Light& Scene::AddLight(const Light& light)
{
    lights.push_back(light);
    return lights.back();
}

void Scene::AddGameObject(std::unique_ptr<GameObject>& object)
{
    object->RegisterComponentFunctions();
    gameObjects[object->GetId()] = std::move(object);
}

void Scene::RemoveGameObject(GameObject* object)
{
    gameObjects.erase(object->GetId());
}

void Scene::SetAddSceneEventCallback(AddEvent func)
{
    addSceneEvent = func;
}

void Scene::UpdateCamera()
{
    std::lock_guard<std::mutex> lg(cameraSwitchMutex);
    camera->CalculateInput();
    camera->Move();
    camera->UpdateMatrix();
}

const ICamera& Scene::GetCamera() const
{
    std::lock_guard<std::mutex> lg(cameraSwitchMutex);
    return *camera;
}

void Scene::SetCamera(std::unique_ptr<ICamera> cam)
{
    std::lock_guard<std::mutex> lg(cameraSwitchMutex);
    camera = std::move(cam);
}

const Light& Scene::GetDirectionalLight() const
{
    return directionalLight;
}

const std::vector<Light>& Scene::GetLights() const
{
    return lights;
}
void Scene::SaveToFile(const std::string& filename)
{
    SaveSceneState(*this, filename);
}
void Scene::LoadFromFile(const std::string& filename)
{
    LoadScene(*this, filename);
}
int Scene::Initialize()
{
    return 0;
}
void Scene::PostInitialize()
{
}
int Scene::Update(float)
{
    return 0;
}
}  // namespace GameEngine
