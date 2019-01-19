#pragma once
#include <string>

namespace GameEngine
{
class Scene;
void SaveSceneState(const Scene& scene, const std::string& filename);

}  // namespace GameEngine
