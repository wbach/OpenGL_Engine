#pragma once
#include <Types.h>

#include "GameEngine/Camera/CameraWrapper.h"

namespace Input
{
class InputManager;
}

namespace common
{
class Transform;
}

namespace GameEngine
{
class Projection;
class DragObject
{
public:
    DragObject(Input::InputManager&, common::Transform&, const CameraWrapper&, const Projection&);
    void Update();

private:
    vec3 WorldToScreenPoint(const vec3& point);
    vec3 ScreenToWorldPoint(const vec3& point);
    float CalculateMouseZCoord(const vec3& objectPosition);
    vec3 GetMouseAsWorldPoint(const vec2& mousePosition, float zCoord);

private:
    Input::InputManager& input_;
    common::Transform& transform_;
    const CameraWrapper& camera_;
    const Projection& projection_;
    vec3 offset_;
    float mouseZcoord_;
};
}  // namespace GameEngine
