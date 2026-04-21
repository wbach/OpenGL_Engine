#pragma once
#include <Utils/TreeNodeReadFunctions.h>
#include <Utils/TreeNodeWriteFunctions.h>

#include <string>

namespace GameEngine
{
class Scene;
class GameObject;
class Prefab;

TreeNode createTree(const Scene&);
TreeNode createPrefab(const GameObject&);
void CreatePrefabNode(TreeNode&, const Prefab&);
void Create(TreeNode&, const GameObject&);
}  // namespace GameEngine
