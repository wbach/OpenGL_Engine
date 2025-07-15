#include "SceneFactoryBase.h"

#include <Logger/Log.h>
#include <dlfcn.h>

#include <filesystem>

#include "Scene.hpp"

typedef GameEngine::Scene* (*CreateSceneFromLib)();

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
    File potentialyLibFile("Libs/lib" + name + ".so");
    DEBUG_LOG("Looking for scene lib file : " + potentialyLibFile.GetAbsoultePath());
    if (std::filesystem::exists(potentialyLibFile.GetAbsoultePath()))
    {
        void* sceneLib = dlopen(potentialyLibFile.GetAbsoultePath().c_str(), RTLD_NOW);
        if (sceneLib)
        {
            DEBUG_LOG("Scene lib found");
            auto create = (CreateSceneFromLib) dlsym(sceneLib, "CreateScene");
            if (create)
            {
                std::unique_ptr<Scene> scene(create());
                DEBUG_LOG("Scene loaded from lib: " + scene->GetName());
                SetMenagersAndApi(*scene.get());
                return scene;
            }
        }
        else
        {
            DEBUG_LOG("Scene lib open failed. Using default");
        }
    }
    DEBUG_LOG("Scene lib not found. Using default");
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

const IdMap& SceneFactoryBase::GetAvaiableScenes() const
{
    return idMap_;
}

void SceneFactoryBase::Clear()
{
    idMap_.clear();
    orderMap_.clear();
    scenesMap_.clear();
}
}  // namespace GameEngine
