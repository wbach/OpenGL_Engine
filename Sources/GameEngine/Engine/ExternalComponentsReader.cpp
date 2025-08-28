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
#include <chrono>
#include <filesystem>

typedef const char* (*registerReadFunction)();

namespace
{
#ifdef USE_GNU
using LibHandle = void*;
inline LibHandle LoadLib(const std::string& file)
{
    return dlopen(file.c_str(), RTLD_NOW | RTLD_GLOBAL);
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

void ExternalComponentsReader::LoadSingle(const std::string& inputFile)
{
    LOG_DEBUG << "Component file detected : " << inputFile;
    if (not std::filesystem::exists(inputFile))
    {
        return;
    }
    using namespace std::chrono;
    auto now = system_clock::now();
    auto ms = duration_cast<milliseconds>(now.time_since_epoch()).count();

    std::string file        = EngineConf.files.cache + "/" + std::to_string(ms) + "_" +
                       std::filesystem::path(inputFile).filename().string();
    std::filesystem::copy(inputFile, file, std::filesystem::copy_options::overwrite_existing);
    LibHandle handle = LoadLib(file);
    if (handle)
    {
        auto symbol = GetSymbol(handle, "registerReadFunction");
        if (not symbol)
        {
            LOG_ERROR << "dlsym failed: " << dlerror();
            return;
        }

        auto func = reinterpret_cast<registerReadFunction>(symbol);
        if (func)
        {
            auto name = func();
            externalLibs.insert({inputFile, ComponentLib{.name = name, .handle = handle}});
        }
        else
        {
            LOG_WARN << "GetSymbol registerReadFunction " << inputFile << " failed : " << dlerror();
            std::filesystem::remove(file);
        }
    }
    else
    {
        LOG_WARN << "Open lib " << inputFile << " failed : " << dlerror();
        std::filesystem::remove(file);
    }
}

void ExternalComponentsReader::Reload(const std::string& path)
{
    auto iter = externalLibs.find(path);
    if (iter != externalLibs.end())
    {
        auto& lib = iter->second;
        LOG_DEBUG << "Reload lib: " << path << " lib name = " << lib.name;
        auto instances = removeAllInstanceOfComponent(Components::BaseComponent::GetType(lib.name));
        Components::ReadFunctions::instance().componentsReadFunctions.erase(lib.name);
        UnloadLib(lib.handle);
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

std::vector<std::pair<std::string, std::string>> ExternalComponentsReader::GetLoadedLibs() const
{
    std::vector<std::pair<std::string, std::string>> result;
    result.reserve(externalLibs.size());
    for (const auto& lib : externalLibs)
    {
        result.push_back({lib.first, lib.second.name});
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
        if (auto component = gameObject.InitComponent(instance.nodeToRestore))
        {
            component->ReqisterFunctions();

            if (auto scene = sceneManager.GetActiveScene())
            {
                auto maybeId = component->getRegisteredFunctionId(Components::FunctionType::Awake);
                if (maybeId)
                {
                    scene->getComponentController().callComponentFunction(gameObject.GetId(), Components::FunctionType::Awake,
                                                                          *maybeId);
                }
            }
        }
    }
}
}  // namespace GameEngine
