#pragma once
#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/Controllers/CharacterController.h"
#include <Common/Controllers/CharacterController/CharacterActions.h>

namespace GameEngine
{
namespace Components
{
class PlayerInputController : public BaseComponent
{
public:
    PlayerInputController(const ComponentContext& componentContext, GameObject& gameObject);

    virtual void ReqisterFunctions() override;

private:
    void Init();
    void SubscribeForPushActions();
    void SubscribeForPopActions();
    bool FindState(common::Controllers::CharacterActions::Type type);
    void AddState(common::Controllers::CharacterActions::Type state);
    void RemoveState(common::Controllers::CharacterActions::Type state);
    void SetRunAnim();
    void SetIdleAnim();

private:
    Animator* animator_;
    CharacterController* characterController_;
    std::list<common::Controllers::CharacterActions::Type> states_;

public:
    static ComponentsType type;
};
}  // namespace Components
}  // namespace GameEngine
