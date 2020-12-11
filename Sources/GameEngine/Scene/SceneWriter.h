#pragma once
#include <string>
#include <Utils/XML/XMLUtils.h>

namespace GameEngine
{
class Scene;
class GameObject;
Utils::XmlNode createTree(const Scene&);
Utils::XmlNode createPrefab(const GameObject&);
}  // namespace GameEngine
