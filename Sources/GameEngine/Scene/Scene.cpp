#include "Scene.hpp"
#include "GameEngine/Camera/Camera.h"
#include "GameEngine/Components/ComponentFactory.h"
#include "GameEngine/Display/DisplayManager.hpp"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Engine/EngineMeasurement.h"
#include "GameEngine/Renderers/GUI/GuiRenderer.h"
#include "GameEngine/Renderers/GUI/Text/GuiTextElement.h"
#include "GameEngine/Renderers/GUI/Window/GuiWindow.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "Input/InputManager.h"
#include "Logger/Log.h"
#include "SceneReader.h"
#include "SceneWriter.h"
#include "Utils/Time/Timer.h"
#include "Utils/XML/XMLUtils.h"
#include "Utils/XML/XmlReader.h"
#include "Utils/md5.h"

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

    DEBUG_LOG("End");
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
    if (displayManager_)
    {
        time_.deltaTime = deltaTime;
    }
    if (inputManager_)
    {
        inputManager_->ProcessKeysEvents();
    }
    if (guiManager_)
    {
        guiManager_->Update();
    }

    Update(deltaTime);
    componentController_.Update();
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
    MakeGuiManager([manager](auto& element) { manager->GetGuiRenderer().Subscribe(element); });
    renderersManager_ = manager;
}

GuiTextElement* Scene::CreateGuiText(const std::string& label, const std::string& font, const std::string& str,
                                     uint32 size, uint32 outline)
{
    auto text   = guiTextFactory_->Create(font, str, size, outline);
    auto result = text.get();
    guiManager_->Add(label, std::move(text));
    return result;
}
GuiTextureElement* Scene::CreateGuiTexture(const std::string& label, const std::string& filename)
{
    auto guiTexture = MakeGuiTexture(filename);
    auto result     = guiTexture.get();
    guiManager_->Add(label, std::move(guiTexture));
    return result;
}

GuiWindowElement* Scene::CreateGuiWindow(const std::string& label, const Rect& rect, const std::string& background)
{
    std::cout << __FUNCTION__ << std::endl;
    auto guiWindow = std::make_unique<GuiWindowElement>(renderersManager_->GetProjection().GetWindowSize(), *inputManager_);
    guiWindow->SetRect(rect);

    auto result = guiWindow.get();
    auto backgroundGuiTexture = MakeGuiTexture(background);
    backgroundGuiTexture->SetScale(guiWindow->GetScale());
    renderersManager_->GetGuiRenderer().Subscribe(*backgroundGuiTexture);
    guiWindow->AddChild(std::move(backgroundGuiTexture));
    guiManager_->Add(label, std::move(guiWindow));
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

void Scene::MakeGuiManager(std::function<void(GuiElement&)> subscribe)
{
    guiManager_ = std::make_unique<GuiManager>(subscribe);
}

std::unique_ptr<GuiTextureElement> Scene::MakeGuiTexture(const std::string &filename)
{
    auto texture    = resourceManager_->GetTextureLaoder().LoadTexture(filename);
    return std::make_unique<GuiTextureElement>(renderersManager_->GetProjection().GetWindowSize(), *texture);
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

void ReadGuiText(Utils::XmlNode& node, Scene& scene, uint32& unnamedTextId)
{
    if (node.GetName() != "text")
    {
        return;
    }
    std::string label, font = "GUI/Ubuntu-M.ttf", value = "empty string";
    uint32 fontSize = 10, outline = 0;

    auto paramNode = node.GetChild("label");
    if (paramNode)
    {
        label = paramNode->value_;
    }
    else
    {
        label = "Unnamed_text_" + std::to_string(unnamedTextId++);
    }

    paramNode = node.GetChild("font");
    if (paramNode)
    {
        font = EngineConf_GetFullDataPathAddToRequierd(paramNode->value_);
    }

    paramNode = node.GetChild("value");
    if (paramNode)
    {
        value = paramNode->value_;
    }

    paramNode = node.GetChild("fontSize");
    if (paramNode)
    {
        try
        {
            fontSize = std::stoi(paramNode->value_);
        }
        catch (...)
        {
            ERROR_LOG("Read gui file, parse font size error.");
        }
    }

    paramNode = node.GetChild("outline");
    if (paramNode)
    {
        try
        {
            outline = std::stoi(paramNode->value_);
        }
        catch (...)
        {
            ERROR_LOG("Read gui file, parse outline error.");
        }
    }

    auto text = scene.GuiText(node.GetChild("label")->value_);

    if (not text)
    {
        text = scene.CreateGuiText(label, font, value, fontSize, outline);
    }
    else
    {
        text->SetText(value);
        text->SetFont(font);
        text->SetFontSize(fontSize);
        text->SetOutline(outline);
    }

    DEBUG_LOG("Update text.");
    paramNode = node.GetChild("color");
    if (paramNode)
    {
        auto color = Utils::ReadVec3(*paramNode);
        text->SetColor(color);
    }

    paramNode = node.GetChild("position");
    if (paramNode)
    {
        auto position = Utils::ReadVec2(*paramNode);
        DEBUG_LOG("Update position : \"" + std::to_string(position) + "\"");
        text->SetPostion(position);
    }
    paramNode = node.GetChild("active");
    if (paramNode)
    {
        text->Show(Utils::ReadBool(*paramNode));
    }
}

void ReadGuiTexture(Utils::XmlNode& node, Scene& scene, uint32& unnamedTextId)
{
    if (node.GetName() != "texture")
    {
        return;
    }

    std::string label, filename;

    auto paramNode = node.GetChild("file");

    if (paramNode)
    {
        filename = paramNode->value_;
    }

    paramNode = node.GetChild("label");
    if (paramNode)
    {
        label = paramNode->value_;
    }
    else
    {
        label = "Unnamed_text_" + std::to_string(unnamedTextId++);
    }

    auto texture = scene.GuiTexture(label);

    if (not texture)
    {
        texture = scene.CreateGuiTexture(label, filename);
    }
    else
    {
    }

    paramNode = node.GetChild("color");

    if (paramNode)
    {
        texture->SetColor(Utils::ReadVec3(*paramNode));
    }

    paramNode = node.GetChild("scale");
    if (paramNode)
    {
        texture->SetScale(Utils::ReadVec2(*paramNode));
    }

    paramNode = node.GetChild("position");
    if (paramNode)
    {
        texture->SetPostion(Utils::ReadVec2(*paramNode));
    }

    paramNode = node.GetChild("active");
    if (paramNode)
    {
        texture->Show(Utils::ReadBool(*paramNode));
    }
}
void Scene::ReadGuiFile(const std::string& filename)
{
    Utils::XmlReader reader;

    auto fileContent = Utils::ReadFile(filename);

    if (fileContent.empty())
    {
        return;
    }

    auto md5Value = md5(fileContent);

    if (md5Value == lastGuiFileMd5Value_)
    {
        // DEBUG_LOG("Gui file not changed. Skip.");
        return;
    }

    DEBUG_LOG("Gui file changed. Parsing.");
    lastGuiFileMd5Value_ = md5Value;

    if (not reader.ReadXml(fileContent))
    {
        return;
    }

    uint32 unnameElementId = 0;

    auto guiNode = reader.Get("gui");

    for (auto& node : guiNode->GetChildren())
    {
        DEBUG_LOG("Node : " + node->GetName());

        ReadGuiText(*node, *this, unnameElementId);
        ReadGuiTexture(*node, *this, unnameElementId);
    }
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
