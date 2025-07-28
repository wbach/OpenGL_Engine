#pragma once
#include <GameEngine/Resources/File.h>

#include <memory>

class TreeNode;

namespace GameEngine
{
class Scene;
class GameObject;

void saveSceneToFile(const Scene&);
void saveSceneToFile(const TreeNode&, const File&);
void createAndSavePrefab(const File&, const GameObject&);
GameObject* cloneGameObject(Scene&, const GameObject&);
}  // namespace GameEngine
