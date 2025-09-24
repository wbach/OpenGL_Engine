#include "SceneFactoryBase.h"

#include <Logger/Log.h>
#include <Utils/Variant.h>
#include <Utils/XML/XmlReader.h>
#include <Utils/XML/XmlWriter.h>

#include <Utils/FileSystem/FileSystemUtils.hpp>
#include <filesystem>

#include "Scene.hpp"
#include "SceneDef.h"
#include "SceneReader.h"

typedef GameEngine::Scene* (*CreateSceneFromLib)();

#ifdef USE_GNU
#include <dlfcn.h>
#else
#include <windows.h>
#endif

namespace GameEngine
{
ScenePtr SceneFactoryBase::Create(const std::string& sceneName)
{
    if (not IsExist(sceneName))
    {
        LOG_DEBUG << "SceneFactory::Create scene : " << sceneName << " not found.";
        return nullptr;
    }

    return GetScene(sceneName);
}
ScenePtr SceneFactoryBase::Create(uint32 id)
{
    if (not IsExist(id))
    {
        LOG_DEBUG << "SceneFactory::Create scene id: " << id << " not found.";
        return nullptr;
    }

    return GetScene(GetSceneName(id));
}

ScenePtr SceneFactoryBase::CreateSceneBasedOnFile(const File& file)
{
    std::string sceneName = file.GetBaseName();
    Utils::XmlReader xmlReader;
    TreeNode* sceneNode{nullptr};
    std::unique_ptr<Scene> scene;

    if (xmlReader.Read(file.GetAbsolutePath().string()))
    {
        sceneNode = xmlReader.Get(CSTR_SCENE);
        if (sceneNode)
        {
            auto maybeNameAtt = sceneNode->attributes_.find("name");
            if (maybeNameAtt != sceneNode->attributes_.end())
            {
                sceneName = maybeNameAtt->second;
            }

            auto maybeSceneFile = sceneNode->getChild("file");

            if (maybeSceneFile)
            {
                File potentialyLibFile(maybeSceneFile->value_);
                if (std::filesystem::exists(potentialyLibFile.GetAbsolutePath()))
                {
#ifdef USE_GNU
                    void* sceneLib = dlopen(potentialyLibFile.GetAbsolutePath().c_str(), RTLD_NOW);
#else
                    HMODULE sceneLib = LoadLibraryA(potentialyLibFile.GetAbsolutePath().string().c_str());
#endif
                    if (sceneLib)
                    {
                        LOG_DEBUG << "Scene lib found";
#ifdef USE_GNU
                        auto create = (CreateSceneFromLib)dlsym(sceneLib, "CreateScene");
#else
                        auto create = (CreateSceneFromLib)GetProcAddress(sceneLib, "CreateScene");
#endif
                        if (create)
                        {
                            scene = std::unique_ptr<Scene>(create());
                            LOG_DEBUG << "Scene loaded from lib: " << scene->GetName();
                        }
                    }
                    else
                    {
                        LOG_DEBUG << "Scene lib open failed. Using default";
                    }
                }
            }
        }
    }

    if (not scene)
    {
        scene = std::make_unique<Scene>(sceneName);
    }

    SetMenagersAndApi(*scene.get());

    if (sceneNode)
    {
        SceneReader(*scene).readNode(*sceneNode);
    }
    return scene;
}
ScenePtr SceneFactoryBase::GetScene(const std::string& name)
{
    auto iter = scenesMap_.find(name);
    if (iter != scenesMap_.end())
    {
        auto [_, method] = *iter;
        auto scene       = std::visit(visitor{[&](const CreateFunction& createFunc)
                                        {
                                            auto scene = createFunc();
                                            scene->InitResources(*engineContext_);
                                            return scene;
                                        },
                                        [&](const File& file) { return CreateSceneBasedOnFile(file); }},
                                      method);

        return scene;
    }

    return nullptr;
}
void SceneFactoryBase::SetMenagersAndApi(Scene& scene)
{
    scene.InitResources(*engineContext_);
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
