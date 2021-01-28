#pragma once
#include <string>
#include <Utils/TreeNode.h>

namespace GameEngine
{
class Scene;
class GameObject;
TreeNode createTree(const Scene&);
TreeNode createPrefab(const GameObject&);
}  // namespace GameEngine
