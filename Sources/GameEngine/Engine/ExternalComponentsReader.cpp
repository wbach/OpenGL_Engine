#include "ExternalComponentsReader.h"

#include <Logger/Log.h>

#include <Utils/FileSystem/FileSystemUtils.hpp>
#include <algorithm>

#include "Configuration.h"
#include "GameEngine/Components/ComponentType.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Components/IComponent.h"
#include "GameEngine/Components/UnknownExternalComponent.h"
#include "GameEngine/Resources/File.h"

#ifdef USE_GNU
#include <dlfcn.h>
#else
#define WIN32_LEAN_AND_MEAN
#include <Utils/Windows.hpp>
#endif

#include <GameEngine/Scene/ISceneManager.h>

#include <GameEngine/Scene/Scene.hpp>
#include <chrono>
#include <filesystem>
#include <unordered_map>

typedef const char* (*registerReadFunction)();

namespace
{
#ifdef USE_GNU
using LibHandle = void*;
inline LibHandle LoadLib(const std::string& file)
{
    return dlopen(file.c_str(), RTLD_NOW | RTLD_LOCAL);
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
inline std::string LastLibError()
{
    const char* err = dlerror();
    return err ? std::string(err) : std::string();
}
#else
using LibHandle = HMODULE;
inline LibHandle LoadLib(const std::filesystem::path& file)
{
    return LoadLibraryA(file.string().c_str());  // wymuszamy wersj� ANSI
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
inline std::string LastLibError()
{
    DWORD errCode = GetLastError();
    if (!errCode)
        return {};

    LPSTR buf  = nullptr;
    DWORD size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
                                errCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPSTR>(&buf), 0, NULL);

    std::string msg(buf, size);
    LocalFree(buf);
    return msg;
}
#endif

using FilePath = std::filesystem::path;
struct ComponentLib
{
    GameEngine::Components::ComponentType type;
    std::string cachedName;
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

ExternalComponentsReader::~ExternalComponentsReader()
{
    LOG_DEBUG << "~ExternalComponentsReader";
    for (auto& [_, lib] : externalLibs)
    {
        removeAllInstanceOfComponent(lib.type.id);
        Components::unregsiterComponentReadFunction(lib.type.name);

        UnloadLib(lib.handle);

        removeCachedFile(lib.cachedName);
    }
    externalLibs.clear();
}

void ExternalComponentsReader::LoadAll()
{
    auto files = getAllComponentFiles();

    for (const auto& file : files)
    {
        LoadSingle(file);
    }
}

void ExternalComponentsReader::LoadSingle(const std::filesystem::path& inputFile)
{
    LOG_DEBUG << "Component file detected : " << inputFile;
    if (not std::filesystem::exists(inputFile))
    {
        return;
    }

    auto file = EngineConf.files.cache / (Utils::CreateUniqueFilename() + "_" + std::filesystem::path(inputFile).filename().string());
    std::filesystem::copy(inputFile, file, std::filesystem::copy_options::overwrite_existing);
    LOG_DEBUG << "LoadLib cached: " << file;
    LibHandle handle = LoadLib(file);
    if (handle)
    {
        auto symbol = GetSymbol(handle, "registerReadFunction");
        if (not symbol)
        {
            LOG_ERROR << "dlsym failed: " << LastLibError();
            return;
        }

        auto func = reinterpret_cast<registerReadFunction>(symbol);
        if (func)
        {
            auto name = func();
            auto id   = Components::getComponentTypeIdByName(name);
            externalLibs.insert(
                {inputFile, ComponentLib{.type = {.id = *id, .name = name}, .cachedName = file.filename().string(), .handle = handle}});
        }
        else
        {
            LOG_WARN << "GetSymbol registerReadFunction " << inputFile << " failed : " << LastLibError();
            std::filesystem::remove(file);
        }
    }
    else
    {
        LOG_WARN << "Open lib " << inputFile << " failed : " << LastLibError();
        std::filesystem::remove(file);
    }
}

void ExternalComponentsReader::Reload(const std::filesystem::path& path)
{
    auto iter = externalLibs.find(path);
    if (iter != externalLibs.end())
    {
        auto& lib = iter->second;
        LOG_DEBUG << "Reload lib: " << path << " lib name = " << lib.type;
        auto instances = removeAllInstanceOfComponent(lib.type.id);
        LOG_DEBUG << "Instances removed : " << instances;
        Components::unregsiterComponentReadFunction(lib.type.name);

        UnloadLib(lib.handle);
        removeCachedFile(lib.cachedName);
        externalLibs.erase(path);
        LoadSingle(path);
        recreateAllInstancesOfComponent(instances);
    }
    else
    {
        LOG_DEBUG << "Component " << path << " not registered. Try load as new one.";
        LoadSingle(path);
    }

    reloadUnknownComponents();
}

void ExternalComponentsReader::reloadUnknownComponents()
{
    if (not sceneManager.GetActiveScene())
        return;

    auto components =
        sceneManager.GetActiveScene()->getComponentController().GetAllComponentsOfType<Components::UnknownExternalComponent>();

    LOG_DEBUG << "Found " << components.size() << " unknnown components";

    for (auto& unknowComponent : components)
    {
        if (not unknowComponent)
            continue;

        auto iter = std::find_if(externalLibs.begin(), externalLibs.end(),
                                 [&unknowComponent](const auto& externalLibPair)
                                 { return externalLibPair.second.type.name == unknowComponent->GetOrginalComponentName(); });

        if (iter != externalLibs.end())
        {
            LOG_DEBUG << unknowComponent->GetTypeName() << " became available. Recreate component of this type.";

            auto goId = unknowComponent->getParentGameObject().GetId();
            if (auto gameObject = sceneManager.GetActiveScene()->GetGameObject(goId))
            {
                TreeNode node("Component");
                unknowComponent->write(node);

                gameObject->RemoveComponent(*unknowComponent);

                if (auto component = gameObject->AddComponent(node))
                {
                    if (auto scene = sceneManager.GetActiveScene())
                    {
                        auto maybeId = component->getRegisteredFunctionId(Components::FunctionType::Awake);
                        if (maybeId)
                        {
                            scene->getComponentController().callComponentFunction(gameObject->GetId(),
                                                                                  Components::FunctionType::Awake, *maybeId);
                        }
                    }
                }
            }
        }
    }
}

void ExternalComponentsReader::removeCachedFile(const std::filesystem::path& cachedName)
{
    try
    {
        std::filesystem::remove(cachedName);
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        LOG_WARN << "Failed to remove cache file " << cachedName << ": " << e.what();
    }
}

std::vector<std::filesystem::path> ExternalComponentsReader::getAllComponentFiles() const
{
    LOG_DEBUG << "Check for ExternalComponents";
    std::string libExtension{".so"};
    const auto componentsDir = EngineConf.files.data / "Components";
    if (not Utils::DirectoryExist(componentsDir))
    {
        LOG_DEBUG << "Components dir not exist : " << componentsDir;
        return {};
    }

    return Utils::FindFilesWithExtension(componentsDir, libExtension);
}

void ExternalComponentsReader::ReloadAll()
{
    auto files = getAllComponentFiles();

    auto externalLibsCopy = externalLibs;
    for (const auto& [file, _] : externalLibsCopy)
    {
        Reload(file);

        auto iter = std::find_if(files.begin(), files.end(), [&file](const auto& path) { return (path == file); });

        if (iter != files.end())
        {
            files.erase(iter);
        }
    }

    for (const auto& file : files)
    {
        LoadSingle(file);
    }

    reloadUnknownComponents();
}

std::vector<std::pair<std::filesystem::path, std::string>> ExternalComponentsReader::GetLoadedLibs() const
{
    std::vector<std::pair<std::filesystem::path, std::string>> result;
    result.reserve(externalLibs.size());
    for (const auto& lib : externalLibs)
    {
        result.push_back({lib.first, lib.second.type.name});
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
        if (auto component = gameObject.AddComponent(instance.nodeToRestore))
        {
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

std::ostream& operator<<(std::ostream& os, const GameEngine::ExternalComponentsReader::ComponentInstance& instance)
{
    os << "ComponentInstance gameOBject(" << instance.gameObject.GetId() << ") name=" << instance.gameObject.GetName();
    return os;
}