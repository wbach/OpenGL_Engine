#include "ExternalComponentsReader.h"

#include <Logger/Log.h>

#include <Utils/FileSystem/FileSystemUtils.hpp>

#include "Configuration.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#ifndef USE_GNU
#ifndef USE_MINGW  // TO DO
#include <DirectXApi/DirectXApi.h>
#endif
#include <shlobj.h>
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#include <GameEngine/Scene/ISceneManager.h>

#include <GameEngine/Scene/Scene.hpp>

typedef const char* (*registerReadFunction)();

namespace
{
#ifdef USE_GNU
using LibHandle = void*;
inline LibHandle LoadLib(const std::string& file)
{
    return dlopen(file.c_str(), RTLD_NOW);
}
inline void UnloadLib(LibHandle lib)
{
    if (lib)
        dlclose(lib);
}
inline void* GetSymbol(LibHandle lib, const char* name)
{
    return dlsym(lib, name);
}
#else
using LibHandle = HMODULE;
inline LibHandle LoadLib(const std::string& file)
{
    return LoadLibrary(file.c_str());
}
inline void UnloadLib(LibHandle lib)
{
    if (lib)
        FreeLibrary(lib);
}
inline void* GetSymbol(LibHandle lib, const char* name)
{
    return reinterpret_cast<void*>(GetProcAddress(lib, name));
}
#endif

using FilePath = std::string;
struct ComponentLib
{
    const char* name;
    LibHandle handle;
};
std::unordered_map<FilePath, ComponentLib> externalLibs;
}  // namespace

namespace GameEngine
{
ExternalComponentsReader::ExternalComponentsReader(ISceneManager& sceneManager)
    : sceneManager(sceneManager)
{
    LoadAll();
}

void ExternalComponentsReader::LoadAll()
{
    LOG_DEBUG << "Check for ExternalComponents";
    std::string libExtension{".so"};
    const auto componentsDir = EngineConf.files.data + "/Components";
    if (not Utils::DirectoryExist(componentsDir))
    {
        LOG_DEBUG << "Components dir not exist : " << componentsDir;
        return;
    }

    const auto& files = Utils::FindFilesWithExtension(componentsDir, libExtension);
    LOG_DEBUG << "Found files : " << files.size();

    for (const auto& file : files)
    {
        LoadSingle(file);
    }
}

void ExternalComponentsReader::LoadSingle(const std::string& file)
{
    LOG_DEBUG << "Component file detected : " << file;
    LibHandle handle = LoadLib(file);
    if (handle)
    {
        auto func = reinterpret_cast<registerReadFunction>(GetSymbol(handle, "registerReadFunction"));
        if (func)
        {
            auto name = func();
            externalLibs.insert({file, ComponentLib{.name = name, .handle = handle}});
            LOG_DEBUG << "Component loaded";
        }
        else
        {
            LOG_WARN << "GetSymbol registerReadFunction " << file << " failed : " << dlerror();
        }
    }
    else
    {
        LOG_WARN << "Open lib " << file << " failed : " << dlerror();
    }
}

void ExternalComponentsReader::Reload(const std::string& path)
{
    auto iter = externalLibs.find(path);
    if (iter != externalLibs.end())
    {
        LOG_DEBUG << "Reload lib: " << path;
        auto& lib      = iter->second;
        auto instances = removeAllInstanceOfComponent(Components::BaseComponent::GetType(lib.name));

        UnloadLib(lib.handle);
        Components::ReadFunctions::instance().componentsReadFunctions.erase(lib.name);
        externalLibs.erase(path);
        LoadSingle(path);

        recreateAllInstancesOfComponent(instances);
    }
    else
    {
        LOG_DEBUG << "Component " << path << " not registered. Try load as new one.";
        LoadSingle(path);
    }
}

std::vector<std::string> ExternalComponentsReader::GetLoadedLibs() const
{
    std::vector<std::string> result;
    result.reserve(externalLibs.size());
    for(const auto& lib : externalLibs)
    {
        result.push_back(lib.first);
    }
    return result;
}

std::vector<ExternalComponentsReader::ComponentInstance> ExternalComponentsReader::removeAllInstanceOfComponent(uint64_t type)
{
    std::vector<ComponentInstance> result;

    if (auto scene = sceneManager.GetActiveScene())
    {
        auto removeComponentsInAllGameObjectInScene = [&](auto&& self, GameObject& gameObject) -> void
        {
            if (auto component = gameObject.GetComponent(type))
            {
                TreeNode node("component");
                component->write(node);
                result.push_back(ComponentInstance{.gameObject = gameObject, .nodeToRestore = std::move(node)});
                component->CleanUp();
                gameObject.RemoveComponent(type);
            }

            for (auto& child : gameObject.GetChildren())
            {
                self(self, *child);
            }
        };

        for (auto& gameObject : scene->GetGameObjects())
        {
            removeComponentsInAllGameObjectInScene(removeComponentsInAllGameObjectInScene, *gameObject);
        }
    }
    return result;
}

void ExternalComponentsReader::recreateAllInstancesOfComponent(const std::vector<ComponentInstance>& instances)
{
    for (auto& instance : instances)
    {
        auto& gameObject = instance.gameObject;
        gameObject.InitComponent(instance.nodeToRestore);
    }
}
}  // namespace GameEngine
