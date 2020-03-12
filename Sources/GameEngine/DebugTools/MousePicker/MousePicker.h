#pragma once
#include "GameEngine/Camera/CameraWrapper.h"
#include "GameEngine/Renderers/Projection.h"
#include <optional>

namespace GameEngine
{
class GameObject;

class MousePicker
{
public:
    MousePicker(const CameraWrapper&, const Projection&, const vec2ui&);
    GameObject* SelectObject(const vec2& mousePosition, const std::vector<std::unique_ptr<GameObject>>&);

private:
    GameObject* Intersect(const std::vector<std::unique_ptr<GameObject>>&, const vec3& ray);
    vec3 CalculateMouseRay(const vec2&);
    vec4 ConvertToEyeCoords(const vec4& clipCoords);
    vec3 ConvertToWorldCoords(const vec4& eyeCoords);
    std::optional<float> Intersect(const GameObject&, const vec3&);
    float CalculateBoundingSphereRadius(const GameObject&);

private:
    const CameraWrapper& camera_;
    const Projection& projection_;
    const vec2ui& windowSize_;
};
}  // namespace GameEngine