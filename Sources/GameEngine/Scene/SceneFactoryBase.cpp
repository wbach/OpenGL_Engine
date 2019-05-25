#include "SceneFactoryBase.h"
#include "Logger/Log.h"
#include "Scene.hpp"

namespace GameEngine
{
ScenePtr SceneFactoryBase::Create(const std::string& sceneName)
{
    if (!IsExist(sceneName))
    {
        DEBUG_LOG("SceneFactory::Create scene : " + sceneName + " not found.");
        return nullptr;
    }

    return GetScene(sceneName);
}
ScenePtr SceneFactoryBase::Create(uint32 id)
{
    if (!IsExist(id))
    {
        DEBUG_LOG("SceneFactory::Create scene id: " + std::to_string(id) + " not found.");
        return nullptr;
    }

    return GetScene(GetSceneName(id));
}
void SceneFactoryBase::SetGraphicsApi(GraphicsApi::IGraphicsApi& graphicsApi)
{
    graphicsApi_ = &graphicsApi;
}
void SceneFactoryBase::SetInputManager(Input::InputManager* input)
{
    input_ = input;
}
void SceneFactoryBase::SetDisplayManager(DisplayManager* displayManager)
{
    displayManager_ = displayManager;
}
void SceneFactoryBase::SetRenderersManager(Renderer::RenderersManager* manager)
{
    rendererMandager_ = manager;
}
void SceneFactoryBase::SetPhysicsApi(Physics::IPhysicsApi& physicsApi)
{
    physicsApi_ = &physicsApi;
}
ScenePtr SceneFactoryBase::GetScene(const std::string& name)
{
    auto scene = scenesMap_[name]();
    SetMenagersAndApi(scene.get());
    return scene;
}
void SceneFactoryBase::SetMenagersAndApi(Scene* scene)
{
    if (not graphicsApi_)
    {
        ERROR_LOG("graphicsApi not set!.");
        return;
    }

    SceneInitContext context
    {
        graphicsApi_,
        input_,
        displayManager_,
        rendererMandager_,
        physicsApi_
    };

    scene->InitResources(context);
}
void SceneFactoryBase::AddScene(const std::string& sceneName, CreateFunction func)
{
    auto currentId = scenesMap_.size();

    idMap_[sceneName]     = currentId;
    orderMap_[currentId]  = sceneName;
    scenesMap_[sceneName] = func;
}
const std::string& SceneFactoryBase::GetSceneName(uint32 id)
{
    return orderMap_[id];
}
uint32 SceneFactoryBase::GetSceneId(const std::string& name)
{
    return idMap_[name];
}
bool SceneFactoryBase::IsExist(uint32 id) const
{
    return orderMap_.count(id) > 0;
}
bool SceneFactoryBase::IsExist(const std::string& name) const
{
    return idMap_.count(name) > 0;
}
}  // namespace GameEngine
