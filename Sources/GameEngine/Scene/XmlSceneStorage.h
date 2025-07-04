#pragma once
#include "ISceneStorage.h"
#include <Utils/TreeNode.h>
#include <optional>

namespace GameEngine
{
class Scene;
class XmlSceneStorage : public ISceneStorage
{
public:
    XmlSceneStorage(Scene&);
    ~XmlSceneStorage() override;

    void store() override;
    void restore() override;
    void saveToFile(const File&) override;
    void readFromFile(const File&) override;
    GameObject* loadPrefab(const File&, const std::string& = "") override;
    void createPrefab(const File&, const GameObject&) override;
    GameObject* clone(const GameObject&) override;

private:
    Scene& scene_;
    std::optional<TreeNode> rootNode_;
    std::optional<TreeNode> maybeModifyObjectsTreeNode_;
};
}  // namespace GameEngine
