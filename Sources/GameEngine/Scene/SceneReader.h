#pragma once
#include <string>

namespace GameEngine
{
class Scene;

namespace SceneReader
{
void LoadScene(Scene& scene, const std::string& filename);
void LoadPrefab(Scene& scene, const std::string& filename, const std::string& name);
}  // namespace SceneReader
}  // namespace GameEngine
