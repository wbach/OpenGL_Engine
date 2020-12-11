#pragma once
#include <Utils/XML/XmlNode.h>
#include "GameEngine/Resources/File.h"
#include <string>

namespace GameEngine
{
class Scene;
class GameObject;

namespace SceneReader
{
void readNode(const Utils::XmlNode& , Scene&);
void loadScene(Scene&, const File&);
GameObject* loadPrefab(Scene&, const File&, const std::string&);
GameObject* createGameObjectFromPrefabNode(Scene&, const Utils::XmlNode&, const std::string&);
}  // namespace SceneReader
}  // namespace GameEngine
