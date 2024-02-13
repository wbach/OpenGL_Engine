#include "RotateableRunState.h"

#include <Input/InputManager.h>
#include <Logger/Log.h>

#include "GameEngine/Display/DisplayManager.hpp"

#include "GameEngine/Camera/CustomCamera.h"
#include "GameEngine/Components/Camera/ThridPersonCamera/ThridPersonCameraComponent.h"
#include "GameEngine/Objects/GameObject.h"

namespace GameEngine
{
namespace Components
{
namespace Camera
{
RotateableRunState::RotateableRunState(Context& context)
    : StateBase(context, context.cameraPositions.run)
    , referenceRelativeCamerePosition{context.cameraPositions.run, 1.f}
    , mouseSensitivity_(.4f)
    , yawLimit{-75.f, 45.f}
    , pitchLimit{-40.f, 50.f}
    , mouseInactivityTimer{2.f}
{
}

RotateableRunState::~RotateableRunState()
{
}

void RotateableRunState::onEnter()
{
    mouseInactiveTime = 0.f;
    StateBase::onEnter();
}

void RotateableRunState::onEnter(const MouseMoveEvent& event)
{
    updatePitchYaw(vec2(event.move.x, event.move.y));
}

void RotateableRunState::update()
{
    const vec2 mouseMove = CalcualteMouseMove() * mouseSensitivity_;
    mouseInactiveTime += context.displayManager.GetTime().deltaTime;

    if (mouseInactiveTime > mouseInactivityTimer)
    {
        pushEventToQueue(MouseInactivityEvent{});
        return;
    }

    updatePitchYaw(mouseMove);
    StateBase::cameraUpdate(context.pitch, context.yaw);
}

vec2 RotateableRunState::CalcualteMouseMove()
{
    auto v = context.inputManager.CalcualteMouseMove();

    if (v.x > 0 or v.y > 0)
    {
        mouseInactiveTime = 0.f;
    }

    return vec2(v.x, v.y);
}
void RotateableRunState::lockPitch()
{
    if (context.pitch < pitchLimit.x)
        context.pitch = pitchLimit.x;
    if (context.pitch > pitchLimit.y)
        context.pitch = pitchLimit.y;
}

void RotateableRunState::updateYaw()
{
    if (context.yaw > 180.f)
    {
        context.yaw  -= 360.f;
    }
    if (context.yaw < -180.f)
    {
        context.yaw  += 360.f;
    }
}

void RotateableRunState::updatePitchYaw(const vec2& mouseMove)
{
    context.yaw  += mouseMove.x;
    context.pitch += mouseMove.y;

    lockPitch();
    updateYaw();
}
}  // namespace Camera
}  // namespace Components
}  // namespace GameEngine
