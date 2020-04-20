#pragma once
#include <string>

namespace GameEngine
{
class Scene;
class GameObject;

namespace SceneReader
{
void LoadScene(Scene& scene, const std::string& filename);
GameObject* LoadPrefab(Scene& scene, const std::string& filename, const std::string& name);
}  // namespace SceneReader
}  // namespace GameEngine
