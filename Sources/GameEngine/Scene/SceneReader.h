#pragma once

#include <memory>
#include <optional>
#include <string>

class TreeNode;

namespace common
{
class Transform;
}

namespace common
{
class Transform;
}

namespace GameEngine
{
class Scene;
class File;
class GameObject;
class Prefab;

class SceneReader
{
public:
    SceneReader(Scene&);

    void readNode(const TreeNode&);
    std::optional<TreeNode> loadScene(const File&);
    std::unique_ptr<Prefab> readPrefab(const File&, const std::string& = "");
    GameObject* loadPrefab(const File&, const std::string& = "");
    GameObject* createGameObjectFromPrefabNodeInRootNode(const TreeNode&, const std::string&);
    std::unique_ptr<GameObject> createGameObjectFromPrefabNode(const TreeNode&, const std::string&);
    void Read(const TreeNode&, GameObject&);
    std::unique_ptr<Prefab> createPrefabGameObject(const TreeNode&);

private:
    template <typename T>
    void AddComponent(const TreeNode&, GameObject&);

    std::unique_ptr<GameObject> createGameObject(const TreeNode&);
    void Read(const TreeNode&, common::Transform&);
    void ReadPrefab(const File&, Prefab&);

private:
    Scene& scene;
};
}  // namespace GameEngine
