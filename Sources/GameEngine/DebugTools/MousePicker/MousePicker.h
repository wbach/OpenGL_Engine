#pragma once
#include "GameEngine/Camera/CameraWrapper.h"
#include "GameEngine/Renderers/Projection.h"

namespace GameEngine
{
class GameObject;

class MousePicker
{
public:
    MousePicker(CameraWrapper&, Projection&);
    GameObject* SelectObject(std::vector<std::unique_ptr<GameObject>>&);

private:
    CameraWrapper& camera_;
    Projection& projection_;
};
}  // namespace GameEngine