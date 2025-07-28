#include "SceneFactoryBase.h"

#include <Logger/Log.h>
#include <Utils/Variant.h>
#include <Utils/XML/XmlReader.h>
#include <Utils/XML/XmlWriter.h>

#include <Utils/FileSystem/FileSystemUtils.hpp>
#include <filesystem>

#ifdef USE_GNU
#include <dlfcn.h>
#else
#include <windows.h>
#endif

#include "Scene.hpp"
#include "SceneDef.h"
#include "SceneReader.h"

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

ScenePtr SceneFactoryBase::CreateSceneBasedOnFile(const File& file)
{
    std::string sceneName = file.GetBaseName();
    Utils::XmlReader xmlReader;
    TreeNode* sceneNode{nullptr};
    std::unique_ptr<Scene> scene;

    if (xmlReader.Read(file.GetAbsoultePath()))
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
                if (std::filesystem::exists(potentialyLibFile.GetAbsoultePath()))
                {
#ifdef USE_GNU
                    void* sceneLib = dlopen(potentialyLibFile.GetAbsoultePath().c_str(), RTLD_NOW);
#else
                    HMODULE sceneLib = LoadLibrary("plugin.dll");
#endif
                    if (sceneLib)
                    {
                        DEBUG_LOG("Scene lib found");
#ifdef USE_GNU
                        auto create = (CreateSceneFromLib)dlsym(sceneLib, "CreateScene");
#else
                        auto create = (CreateSceneFromLib)GetProcAddress(sceneLib, "CreateScene");
#endif
                        if (create)
                        {
                            scene = std::unique_ptr<Scene>(create());
                            DEBUG_LOG("Scene loaded from lib: " + scene->GetName());
                        }
                    }
                    else
                    {
                        DEBUG_LOG("Scene lib open failed. Using default");
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
        SceneReader::readNode(*sceneNode, *scene);
    }
    return scene;
}
ScenePtr SceneFactoryBase::GetScene(const std::string& name)
{
    auto iter = scenesMap_.find(name);
    if (iter != scenesMap_.end())
    {
        auto [_, method] = *iter;
        auto scene       = std::visit(
            visitor{
                [&](const CreateFunction& createFunc)
                {
                    auto scene = createFunc();
                    scene->InitResources(*engineContext_);
                    return scene;
                },
                [&](const File& file) { return CreateSceneBasedOnFile(file); },
                [](const std::monostate&)
                {
                    ERROR_LOG("Image data is not set!");
                    return nullptr;
                },
            },
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
