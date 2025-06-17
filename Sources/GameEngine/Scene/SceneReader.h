#pragma once
#include <Utils/TreeNode.h>
#include "GameEngine/Resources/File.h"
#include <string>

namespace GameEngine
{
class Scene;
class GameObject;

namespace SceneReader
{
void readNode(const TreeNode& , Scene&);
void loadScene(Scene&, const File&);
void loadPrefab(Scene&, const File&);
GameObject* createGameObjectFromPrefabNodeInRootNode(Scene&, const TreeNode&, const std::string&);
std::unique_ptr<GameObject> createGameObjectFromPrefabNode(Scene&, const TreeNode&, const std::string&);
}  // namespace SceneReader
}  // namespace GameEngine
