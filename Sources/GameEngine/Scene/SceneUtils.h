#pragma once
#include <GameEngine/Resources/File.h>
#include <unordered_map>

class TreeNode;

namespace GameEngine
{
class Scene;
class GameObject;

void saveSceneToFile(Scene&);
void saveSceneToFile(Scene&, const File&);
void saveSceneToFile(const TreeNode&, const File&);
void createAndSavePrefab(const File&, const GameObject&);
GameObject* cloneGameObject(Scene&, const GameObject&);
void createScenesFile(const File&);
void createScenesFile(const File&, const std::unordered_map<std::string, File>&, const std::string&);
void createScenesFile(const File&, const std::unordered_map<std::string, std::filesystem::path>&, const std::string&);
}  // namespace GameEngine
