#pragma once
#include <string>
#include <Utils/XML/XMLUtils.h>

namespace GameEngine
{
class Scene;
Utils::XmlNode createTree(const Scene& scene);
}  // namespace GameEngine
