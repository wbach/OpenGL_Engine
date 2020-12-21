#pragma once
#include <Types.h>
#include <optional>
#include <memory>

namespace GameEngine
{
class GameObject;
class Projection;
class CameraWrapper;

class MousePicker
{
public:
    struct Ray
    {
        vec3 position;
        vec3 direction;
    };

    MousePicker(const CameraWrapper&, const Projection&);
    GameObject* SelectObject(const vec2& mousePosition, const std::vector<std::unique_ptr<GameObject>>&);

private:
    GameObject* Intersect(const std::vector<std::unique_ptr<GameObject>>&, const Ray&);
    std::optional<float> Intersect(const GameObject&, const Ray&);
    float CalculateBoundingSphereRadius(const GameObject&);

private:
    const CameraWrapper& camera_;
    const Projection& projection_;
};
}  // namespace GameEngine
