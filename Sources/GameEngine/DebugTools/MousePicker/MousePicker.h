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
    enum class IntersectMode
    {
        All,
        FrontOnly
    };
    MousePicker(const ICamera&, IntersectMode = IntersectMode::All);
    GameObject* SelectObject(const vec2&, const std::vector<std::unique_ptr<GameObject>>&);

private:
    GameObject* Intersect(const std::vector<std::unique_ptr<GameObject>>&, const Ray&);
    std::optional<float> Intersect(const GameObject&, const Ray&);
    float CalculateBoundingSphereRadius(const GameObject&);
    std::optional<std::pair<GameObject*, float>> IntersectObject(const GameObject*, const Ray&);

private:
    const ICamera& camera_;
    IntersectMode mode;
};
}  // namespace GameEngine
