#pragma once
#include <string>
#include <vector>
#include <Utils/TreeNode.h>

namespace GameEngine
{
class ISceneManager;
class GameObject;

struct ExternalComponentsReader
{
    ExternalComponentsReader(ISceneManager&);
    void LoadAll();
    void LoadSingle(const std::string&);
    void Reload(const std::string&);
    std::vector <std::string> GetLoadedLibs() const;

    struct ComponentInstance
    {
        GameObject& gameObject;
        TreeNode nodeToRestore;
    };
    std::vector<ComponentInstance> removeAllInstanceOfComponent(uint64_t);
    void recreateAllInstancesOfComponent(const std::vector<ComponentInstance>&);

private:
    ISceneManager& sceneManager;
};
}  // namespace GameEngine
