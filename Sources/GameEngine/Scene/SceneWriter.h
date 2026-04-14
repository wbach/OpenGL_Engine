#pragma once
#include <string>
#include <Utils/TreeNodeWriteFunctions.h>
#include <Utils/TreeNodeReadFunctions.h>

namespace GameEngine
{
class Scene;
class GameObject;

TreeNode createTree(const Scene&);
TreeNode createPrefab(const GameObject&);
void Create(TreeNode&, const GameObject&);
}  // namespace GameEngine
