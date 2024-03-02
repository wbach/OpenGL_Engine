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
{
}

void StateBase::onEnter()
{
    context.pitch = 0.f;
    context.yaw   = 0.f;
    setUpdateFunc();
}

void StateBase::setUpdateFunc()
{
    context.camera.setOnUpdate([this]() { update(); });
}

void StateBase::pushEventToQueue(const Event& event) const
{
    if (thridPersonCameraComponent)
    {
        thridPersonCameraComponent->pushEventToQueue(event);
    }
}

void StateBase::cameraUpdate(const mat4& parentWorldTransform, float pitch, float yaw)
{
    auto [relativeCamerePosition, lookAtLocalPosition, yTranslation] = calculateLocalPosition();

    auto rotX = glm::normalize(glm::angleAxis(glm::radians(pitch), glm::vec3(1.f, 0.f, 0.f)));
    auto rotY = glm::normalize(glm::angleAxis(glm::radians(yaw), glm::vec3(0.f, 1.f, 0.f)));

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
}  // namespace Camera
}  // namespace Components
}  // namespace GameEngine
