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
    void LoadSingle(const std::string&);
    void Reload(const std::string&);
    void ReloadAll();

    std::vector<std::pair<std::string, std::string>> GetLoadedLibs() const;

    struct ComponentInstance
    {
        GameObject& gameObject;
        TreeNode nodeToRestore;
    };
    std::vector<ComponentInstance> removeAllInstanceOfComponent(uint64_t);
    void recreateAllInstancesOfComponent(const std::vector<ComponentInstance>&);

private:
    void removeCachedFile(const std::string&);

private:
    ISceneManager& sceneManager;
};
}  // namespace GameEngine

std::ostream& operator<<(std::ostream&, const GameEngine::ExternalComponentsReader::ComponentInstance&);
