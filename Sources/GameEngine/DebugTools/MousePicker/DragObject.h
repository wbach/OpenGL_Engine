#pragma once
#include <Types.h>

#include "GameEngine/Camera/CameraWrapper.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Components/Physics/Rigidbody.h"

namespace Input
{
class InputManager;
}


namespace GameEngine
{
class Projection;
class DragObject
{
public:
    DragObject(Input::InputManager&, GameObject&, const CameraWrapper&, const Projection&);
    ~DragObject();

    void Update();

private:
    vec3 WorldToScreenPoint(const vec3& point);
    vec3 ScreenToWorldPoint(const vec3& point);
    float CalculateMouseZCoord(const vec3& objectPosition);
    vec3 GetMouseAsWorldPoint(const vec2& mousePosition, float zCoord);
    void KeyDirectionLock(vec3& newPosition);

private:
    Input::InputManager& input_;
    GameObject& gameObject_;
    Components::Rigidbody* rigidbody_;
    const CameraWrapper& camera_;
    const Projection& projection_;
    vec3 offset_;
    float mouseZcoord_;

    vec3 cameraStartPos_;
};
}  // namespace GameEngine
