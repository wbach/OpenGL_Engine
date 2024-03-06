#include "StateBase.h"

#include "GameEngine/Camera/CustomCamera.h"
#include "GameEngine/Components/Camera/ThridPersonCamera/ThridPersonCameraComponent.h"
#include "GameEngine/Objects/GameObject.h"

namespace GameEngine
{
namespace Components
{
namespace Camera
{
StateBase::StateBase(Context& context)
    : StateBase(context, vec3(0))
{
}

StateBase::StateBase(Context& context, const vec3& rcp)
    : context{context}
    , thridPersonCameraComponent{context.gameObject.GetComponent<ThridPersonCameraComponent>()}
    , relativeCamerePosition(rcp)
    , mouseSensitivity_{0.2f}
    , pitchLimit{-40.f, 50.f}
    , rotationDir{1.f, 1.f}
{
}

void StateBase::onEnter()
{
    setUpdateFunc();
}

void StateBase::setUpdateFunc()
{
    context.camera.setOnUpdate([this]() { update(); });
}

void StateBase::update()
{
    StateBase::cameraUpdate();
}

void StateBase::pushEventToQueue(const Event& event) const
{
    if (thridPersonCameraComponent)
    {
        thridPersonCameraComponent->pushEventToQueue(event);
    }
}

void StateBase::cameraUpdate()
{
    updatePitchYaw(calculateMouseMove());

    auto parentWorldTransform = glm::translate(context.gameObject.GetWorldTransform().GetPosition()) *
                                glm::scale(context.gameObject.GetWorldTransform().GetScale());

    auto [relativeCamerePosition, lookAtLocalPosition, yTranslation] = calculateLocalPosition();

    auto rotX = glm::normalize(glm::angleAxis(glm::radians(context.pitch), glm::vec3(1.f, 0.f, 0.f)));
    auto rotY = glm::normalize(glm::angleAxis(glm::radians(context.yaw), glm::vec3(0.f, 1.f, 0.f)));

    auto pwt = parentWorldTransform * yTranslation * mat4_cast(rotY * rotX);

    auto worldCameraPosition = pwt * relativeCamerePosition;
    context.camera.SetPosition(worldCameraPosition);

    auto lookAtPosition = pwt * lookAtLocalPosition;
    context.camera.LookAt(lookAtPosition);
}

std::tuple<vec4, vec4, mat4> StateBase::calculateLocalPosition() const
{
    vec4 pos(relativeCamerePosition.x, 0.f, relativeCamerePosition.z, 1.f);
    vec4 lpos(relativeCamerePosition.x, 0.f, 0.f, 1.f);
    mat4 ytranslation(glm::translate(vec3(0.f, relativeCamerePosition.y, 0.f)));
    return {pos, lpos, ytranslation};
}

vec2 StateBase::calculateMouseMove() const
{
    auto mouseMove = context.inputManager.CalcualteMouseMove();
    return vec2(static_cast<float>(-mouseMove.x) * rotationDir.x, static_cast<float>(mouseMove.y) * rotationDir.y) *
           mouseSensitivity_;
}

void StateBase::lockPitch()
{
    if (context.pitch < pitchLimit.x)
        context.pitch = pitchLimit.x;
    if (context.pitch > pitchLimit.y)
        context.pitch = pitchLimit.y;
}

void StateBase::updateYaw()
{
    if (context.yaw > 180.f)
    {
        context.yaw -= 360.f;
    }
    if (context.yaw < -180.f)
    {
        context.yaw += 360.f;
    }
}

void StateBase::updatePitchYaw(const vec2& mouseMove)
{
    context.yaw += mouseMove.x;
    context.pitch += mouseMove.y;

    lockPitch();
    updateYaw();
}
}  // namespace Camera
}  // namespace Components
}  // namespace GameEngine
