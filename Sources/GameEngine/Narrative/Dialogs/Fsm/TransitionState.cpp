#include "TransitionState.h"

#include "DialogContext.h"
#include "GameEngine/Components/Camera/CameraComponent.h"
#include "GameEngine/Components/Camera/ThridPersonCamera/ThridPersonCameraComponent.h"
#include "GameEngine/Components/Dialogue/DialogueComponent.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Scene/TweenManager.h"
#include "Logger/Log.h"

namespace GameEngine
{
TransitionState::TransitionState(DialogContext& context)
    : dialogContext{context}
{
}
void TransitionState::onEnter(const StartRequested& startRequested)
{
    auto current = startRequested.component.getCurrent();
    if (not current)
    {
        LOG_DEBUG << "Dialog not avaiable";
        return;
    }

    auto sendShowEvent = [this, reguest = startRequested, node = *current]()
    {
        dialogContext.sendEvent(
            StartSentence{.dialogNode = node, .playerGameObject = reguest.playerGameObject, .component = reguest.component});
    };

    startRequested.component.RotateObjectToPlayer(startRequested.playerGameObject.GetWorldTransform().GetPosition());

    auto cameraComponent = startRequested.playerGameObject.GetComponentInChild<Components::CameraComponent>();
    if (cameraComponent)
    {
        auto thridPersonCameraComponent = startRequested.playerGameObject.GetComponent<Components::ThridPersonCameraComponent>();
        if (thridPersonCameraComponent)
        {
            thridPersonCameraComponent->pushEventToQueue(Components::Camera::StartScriptedMode{});
        }

        const float duration{0.5f};
        if (auto targetTransform = calculateCameraTarget(startRequested.playerGameObject, startRequested.component))
        {
            dialogContext.tweenManager.Add(cameraComponent->GetParentGameObject(), *targetTransform, duration, EaseType::CubicOut,
                                           sendShowEvent);
        }
        else
        {
            sendShowEvent();
        }
    }
    else
    {
        sendShowEvent();
    }
}
std::optional<TweenTransform> TransitionState::calculateCameraTarget(GameObject& gameObject,
                                                                     Components::DialogueComponent& dialogueComponent)
{
    vec3 playerPos = gameObject.GetWorldTransform().GetPosition();
    vec3 npcPos    = dialogueComponent.GetParentGameObject().GetWorldTransform().GetPosition();

    vec3 diff           = npcPos - playerPos;
    float distanceSq    = dot(diff, diff);
    const float epsilon = 0.0001f;
    if (distanceSq < epsilon)
    {
        LOG_DEBUG << "Player and NPC are in the same position";
        return {};
    }

    vec3 forward = normalize(diff);
    vec3 up      = (std::abs(forward.y) > 0.999f) ? vec3(0, 0, 1) : vec3(0, 1, 0);
    vec3 right   = normalize(cross(up, forward));

    vec3 targetPos   = playerPos - (forward * 0.6f) + (right * 1.6f) + vec3(0, 1.7f, 0);
    vec3 lookAtPoint = npcPos + vec3(0, 1.6f, 0);
    auto targetRot   = Utils::lookAt(lookAtPoint, targetPos);

    TweenTransform target;
    target.position = targetPos;
    target.rotation = targetRot;

    return target;
}
void TransitionState::onEnter(const EndDialog& event)
{
    if (auto thridPersonCameraComponent = event.playerGameObject.GetComponent<Components::ThridPersonCameraComponent>())
    {
        thridPersonCameraComponent->pushEventToQueue(Components::Camera::StopScriptedMode{});
    }
    event.component.RestoreRotation();

    dialogContext.sendEvent(event);
}
}  // namespace GameEngine
