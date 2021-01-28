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
ScenePtr SceneFactoryBase::GetScene(const std::string& name)
{
    auto scene = scenesMap_[name]();
    SetMenagersAndApi(*scene.get());
    return scene;
}
void SceneFactoryBase::SetMenagersAndApi(Scene& scene)
{
    scene.InitResources(*engineContext_);
}
void SceneFactoryBase::AddScene(const std::string& sceneName, CreateFunction func)
{
    auto currentId = static_cast<uint32>(scenesMap_.size());

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

void SceneFactoryBase::SetEngineContext(EngineContext& engineContext)
{
    engineContext_ = &engineContext;
}
}  // namespace GameEngine
