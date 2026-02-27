#pragma once
#include <Input/KeysSubscriptionsManager.h>

#include "GameEngine/Components/BaseComponent.h"

namespace GameEngine
{
namespace Components
{
class CharacterController;
class ENGINE_API PlayerInputController : public BaseComponent
{
public:
    PlayerInputController(ComponentContext& componentContext, GameObject& gameObject);

    void Init();
    void CleanUp() override;
    void ReqisterFunctions() override;
    void Reload() override;
    bool IsInitated() const;

private:
    void SubscribeForPushActions();
    void SubscribeForPopActions();

private:
    CharacterController* characterController_;
    Input::KeysSubscriptionsManager subscriptions_;

    bool isRotateLeftPressed_{false};
    bool isRotateRightPressed_{false};
    bool init{false};

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
