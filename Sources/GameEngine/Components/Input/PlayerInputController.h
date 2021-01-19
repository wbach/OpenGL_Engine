#pragma once
#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/Controllers/CharacterController.h"
#include <Common/Controllers/CharacterController/CharacterActions.h>
#include <Input/KeysSubscriptionsManager.h>

namespace GameEngine
{
namespace Components
{
class PlayerInputController : public BaseComponent
{
public:
    PlayerInputController(ComponentContext& componentContext, GameObject& gameObject);

    void CleanUp() override;
    void ReqisterFunctions() override;

public:
    std::string weaponChildObjectName_;
    std::string weaponBoneName_;
    vec3 weponBonePositionOffset_;
    vec3 weponBoneRotationOffsetDegreesEulers_;

private:
    void Init();
    void SubscribeForPushActions();
    void SubscribeForPopActions();

private:
    Animator* animator_;
    CharacterController* characterController_;
    std::vector<CharacterControllerState::Type> states_;
    Input::KeysSubscriptionsManager subscriptions_;

    std::optional<uint32> connectedBone_;

    std::function<void()> jumpCallback_;

public:
    static ComponentsType type;
};
}  // namespace Components
}  // namespace GameEngine
