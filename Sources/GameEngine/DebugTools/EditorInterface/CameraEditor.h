#pragma once
#include <Input/KeysSubscriptionsManager.h>

#include <optional>

#include "GameEngine/Camera/Camera.h"

namespace Input
{
class InputManager;
}  // namespace Input

namespace GameEngine
{
class DisplayManager;
class BoundingBox;

class CameraEditor : public Camera
{
public:
    CameraEditor(Input::InputManager&, DisplayManager&);
    ~CameraEditor();
    void Update() override;

    void SetPerspectiveView(const std::optional<vec3>& target = std::nullopt);
    void LookAtTop(const std::optional<vec3>& target = std::nullopt);
    void LookAtBottom(const std::optional<vec3>& target = std::nullopt);
    void LookAtFront(const std::optional<vec3>& target = std::nullopt);
    void LookAtBack(const std::optional<vec3>& target = std::nullopt);
    void LookAtLeft(const std::optional<vec3>& target = std::nullopt);
    void LookAtRight(const std::optional<vec3>& target = std::nullopt);
    void ZoomIn();
    void ZoomOut();
    void SetDistanceToFitBoundingBox(const BoundingBox&);

private:
    vec3 CalculateInputs();
    void CalculatePitch(float);
    void CalculateYaw(float);
    vec2 CalcualteMouseMove();
    vec3 ComputeTarget(const std::optional<vec3>&);

private:
    Input::KeysSubscriptionsManager subscriptionsManager_;
    Input::InputManager& inputManager_;
    DisplayManager& displayManager_;
    std::optional<vec2i> referenceMousePosition_;
    bool rotationEnabled_;
    vec3 lastTarget_{0.f};
    float orthoZoom_{1.f};
};
}  // namespace GameEngine
