#pragma once
#include <Types.h>

#include <memory>
#include <optional>

namespace GameEngine
{
class GameObject;
class Projection;
class ICamera;

class MousePicker
{
public:
    struct Ray
    {
        vec3 position;
        vec3 direction;
    };

    MousePicker(const ICamera&);
    GameObject* SelectObject(const vec2&, const std::vector<std::unique_ptr<GameObject>>&);

private:
    GameObject* Intersect(const std::vector<std::unique_ptr<GameObject>>&, const Ray&);
    std::optional<float> Intersect(const GameObject&, const Ray&);
    float CalculateBoundingSphereRadius(const GameObject&);
    std::optional<std::pair<GameObject*, float>> IntersectObject(const GameObject*, const MousePicker::Ray&);

private:
    const ICamera& camera_;
};
}  // namespace GameEngine
