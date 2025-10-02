#pragma once
#include <Input/KeysSubscriptionsManager.h>

#include "GameEngine/Components/BaseComponent.h"

namespace GameEngine
{
namespace Components
{
class CharacterController;
class PlayerInputController : public BaseComponent
{
public:
    PlayerInputController(ComponentContext& componentContext, GameObject& gameObject);

    void CleanUp() override;
    void ReqisterFunctions() override;
    void Reload() override;

private:
    void Init();
    void SubscribeForPushActions();
    void SubscribeForPopActions();

private:
    CharacterController* characterController_;
    Input::KeysSubscriptionsManager subscriptions_;

    bool isRotateLeftPressed_{false};
    bool isRotateRightPressed_{false};

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
