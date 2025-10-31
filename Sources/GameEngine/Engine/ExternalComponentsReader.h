#pragma once
#include <Utils/TreeNode.h>

#include <string>
#include <vector>

namespace GameEngine
{
class ISceneManager;
class GameObject;

struct ExternalComponentsReader
{
    ExternalComponentsReader(ISceneManager&);
    ~ExternalComponentsReader();
    void LoadAll();
    void LoadSingle(const std::filesystem::path&);
    void Reload(const std::filesystem::path&);
    void ReloadAll();

    std::vector<std::pair<std::filesystem::path, std::string>> GetLoadedLibs() const;

    struct ComponentInstance
    {
        GameObject& gameObject;
        TreeNode nodeToRestore;
    };
    std::vector<ComponentInstance> removeAllInstanceOfComponent(uint64_t);
    void recreateAllInstancesOfComponent(const std::vector<ComponentInstance>&);

private:
    void removeCachedFile(const std::filesystem::path&);
    std::vector<std::filesystem::path> getAllComponentFiles() const;
    void reloadUnknownComponents();

private:
    ISceneManager& sceneManager;
};
}  // namespace GameEngine

std::ostream& operator<<(std::ostream&, const GameEngine::ExternalComponentsReader::ComponentInstance&);
