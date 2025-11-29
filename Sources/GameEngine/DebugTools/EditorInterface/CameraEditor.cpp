#include "CameraEditor.h"

#include <Input/InputManager.h>
#include <Logger/Log.h>
#include <Utils/GLM/GLMUtils.h>

#include <memory>

#include "GameEngine/Display/DisplayManager.hpp"
#include "GameEngine/Renderers/Projection/OrthographicProjection.h"
#include "GameEngine/Renderers/Projection/PerspectiveProjection.h"

namespace GameEngine
{
namespace
{
const float defaultCamSpeed         = 10.f;
const float defaultCamRotationSpeed = 0.25f;
const float defaultOrthoDistance    = 20.f;
}  // namespace

CameraEditor::CameraEditor(Input::InputManager& inputManager, DisplayManager& displayManager)
    : Camera(vec3(2.f), vec3(0.f))
    , subscriptionsManager_(inputManager)
    , inputManager_(inputManager)
    , displayManager_(displayManager)
    , rotationEnabled_(true)
{
    subscriptionsManager_ = inputManager_.SubscribeOnKeyDown(
        KeyCodes::RMOUSE, [&]() { referenceMousePosition_ = inputManager_.GetPixelMousePosition(); });

    subscriptionsManager_ = inputManager_.SubscribeOnKeyUp(KeyCodes::RMOUSE, [&]() { referenceMousePosition_ = std::nullopt; });
    subscriptionsManager_ = inputManager_.SubscribeOnKeyDown(KeyCodes::MOUSE_WHEEL, [this]() { ZoomIn(); });
    subscriptionsManager_ = inputManager_.SubscribeOnKeyUp(KeyCodes::MOUSE_WHEEL, [this]() { ZoomOut(); });
}

CameraEditor::~CameraEditor()
{
    subscriptionsManager_.UnsubscribeKeys();
}

void CameraEditor::Update()
{
    if (rotationEnabled_ and referenceMousePosition_)
    {
        auto mouseMove = CalcualteMouseMove() * defaultCamRotationSpeed;
        CalculateYaw(mouseMove.x);
        CalculatePitch(mouseMove.y);
    }

    vec3 moveVector = CalculateInputs();

    if (glm::length(moveVector) > std::numeric_limits<float>::epsilon())
    {
        moveVector = moveVector * glm::normalize(GetRotation().value_);
        moveVector = moveVector * defaultCamSpeed * displayManager_.GetTime().deltaTime;
        IncreasePosition(moveVector);
    }
}

vec3 CameraEditor::CalculateInputs()
{
    vec3 move(0.f);

    if (inputManager_.GetKey(Input::GameAction::CAM_LEFT))
        move.x -= 1.f;
    if (inputManager_.GetKey(Input::GameAction::CAM_RIGHT))
        move.x += 1.f;
    if (inputManager_.GetKey(Input::GameAction::CAM_FORWARD))
        move.z -= 1.f;
    if (inputManager_.GetKey(Input::GameAction::CAM_BACKWARD))
        move.z += 1.f;

    if (rotationEnabled_)
    {
        if (inputManager_.GetKey(Input::GameAction::CAM_TURN_LEFT))
            IncreaseYaw(-1.f);
        if (inputManager_.GetKey(Input::GameAction::CAM_TURN_RIGHT))
            IncreaseYaw(1.f);
        if (inputManager_.GetKey(Input::GameAction::CAM_PITCH_UP))
            IncreasePitch(-1.f);
        if (inputManager_.GetKey(Input::GameAction::CAM_PITCH_DOWN))
            IncreasePitch(1.f);
    }
    else
    {
        const auto currentOrthoForward = vec3(0.f, -1.f, 0.f);
        const auto currentOrthoRight   = vec3(1, 0, 0);
        return currentOrthoRight * move.x + currentOrthoForward * move.z;
    }

    return move;
}

void CameraEditor::CalculatePitch(float v)
{
    IncreasePitch(v);
}

void CameraEditor::CalculateYaw(float v)
{
    IncreaseYaw(v);
}

vec2 CameraEditor::CalcualteMouseMove()
{
    vec2 result;
    auto currentMousePosition = inputManager_.GetPixelMousePosition();
    result.x                  = static_cast<float>(currentMousePosition.x - referenceMousePosition_->x);
    result.y                  = static_cast<float>(currentMousePosition.y - referenceMousePosition_->y);
    referenceMousePosition_   = currentMousePosition;
    return result;
}

vec3 CameraEditor::ComputeTarget(const std::optional<vec3>& maybeTarget)
{
    if (maybeTarget.has_value())
        return maybeTarget.value();

    return position_ + GetForward() * 20.f;
}

void CameraEditor::LookAtTop(const std::optional<vec3>& maybeTarget)
{
    vec3 target    = ComputeTarget(maybeTarget);
    float distance = glm::length(position_ - lastTarget_);
    if (distance < 0.001f)
        distance = defaultOrthoDistance;

    position_ = target + vec3(0.f, distance, 0.f);
    LookAt(target);

    auto ortho = std::make_unique<OrthographicProjection>();
    ortho->SetZoom(orthoZoom_);
    ortho->UpdateMatrix();
    projection_ = std::move(ortho);

    rotationEnabled_     = false;
    lastTarget_          = target;
}

void CameraEditor::LookAtBottom(const std::optional<vec3>& maybeTarget)
{
    vec3 target    = ComputeTarget(maybeTarget);
    float distance = glm::length(position_ - lastTarget_);
    if (distance < 0.001f)
        distance = defaultOrthoDistance;

    position_ = target - vec3(0.f, distance, 0.f);
    LookAt(target);

    auto ortho = std::make_unique<OrthographicProjection>();
    ortho->SetZoom(orthoZoom_);
    ortho->UpdateMatrix();
    projection_ = std::move(ortho);

    rotationEnabled_     = false;
    lastTarget_          = target;
}

void CameraEditor::LookAtFront(const std::optional<vec3>& maybeTarget)
{
    vec3 target    = ComputeTarget(maybeTarget);
    float distance = glm::length(position_ - lastTarget_);
    if (distance < 0.001f)
        distance = defaultOrthoDistance;

    position_ = target + vec3(0.f, 0.f, distance);
    LookAt(target);

    auto ortho = std::make_unique<OrthographicProjection>();
    ortho->SetZoom(orthoZoom_);
    ortho->UpdateMatrix();
    projection_ = std::move(ortho);

    rotationEnabled_     = false;
    lastTarget_          = target;
}

void CameraEditor::LookAtBack(const std::optional<vec3>& maybeTarget)
{
    vec3 target    = ComputeTarget(maybeTarget);
    float distance = glm::length(position_ - lastTarget_);
    if (distance < 0.001f)
        distance = defaultOrthoDistance;

    position_ = target - vec3(0.f, 0.f, distance);
    LookAt(target);

    auto ortho = std::make_unique<OrthographicProjection>();
    ortho->SetZoom(orthoZoom_);
    ortho->UpdateMatrix();
    projection_ = std::move(ortho);

    rotationEnabled_     = false;
    lastTarget_          = target;
}

void CameraEditor::LookAtLeft(const std::optional<vec3>& maybeTarget)
{
    vec3 target    = ComputeTarget(maybeTarget);
    float distance = glm::length(position_ - lastTarget_);
    if (distance < 0.001f)
        distance = defaultOrthoDistance;

    position_ = target - vec3(distance, 0.f, 0.f);
    LookAt(target);

    auto ortho = std::make_unique<OrthographicProjection>();
    ortho->SetZoom(orthoZoom_);
    ortho->UpdateMatrix();
    projection_ = std::move(ortho);

    rotationEnabled_     = false;
    lastTarget_          = target;
}

void CameraEditor::LookAtRight(const std::optional<vec3>& maybeTarget)
{
    vec3 target    = ComputeTarget(maybeTarget);
    float distance = glm::length(position_ - lastTarget_);
    if (distance < 0.001f)
        distance = defaultOrthoDistance;

    position_ = target + vec3(distance, 0.f, 0.f);
    LookAt(target);

    auto ortho = std::make_unique<OrthographicProjection>();
    ortho->SetZoom(orthoZoom_);
    ortho->UpdateMatrix();
    projection_ = std::move(ortho);

    rotationEnabled_     = false;
    lastTarget_          = target;
}

void CameraEditor::SetPerspectiveView(const std::optional<vec3>& maybeTarget)
{
    projection_ = std::make_unique<PerspectiveProjection>();
    projection_->UpdateMatrix();
    rotationEnabled_ = true;

    if (maybeTarget)
        LookAt(*maybeTarget);
}
void CameraEditor::ZoomIn()
{
    if (auto orthographicProjection = dynamic_cast<OrthographicProjection*>(projection_.get()))
    {
        orthoZoom_ += 0.1f;
        orthographicProjection->SetZoom(orthoZoom_);
        projection_->UpdateMatrix();
    }
}
void CameraEditor::ZoomOut()
{
    if (auto orthographicProjection = dynamic_cast<OrthographicProjection*>(projection_.get()))
    {
        orthoZoom_ -= 0.1f;
        orthographicProjection->SetZoom(orthoZoom_);
        projection_->UpdateMatrix();
    }
}
}  // namespace GameEngine
