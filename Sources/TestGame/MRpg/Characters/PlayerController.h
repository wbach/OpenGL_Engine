#pragma once
#include <Common/Hero/Stats.h>
#include <Common/Messages/TransformMessages/TransformMessageTypes.h>
#include <memory>

namespace Input
{
class InputManager;
}

namespace Network
{
class Gateway;
}

namespace MmmoRpg
{
struct MrpgGameContext;

class PlayerController
{
   public:
    PlayerController(Input::InputManager* manager, MrpgGameContext& gameContext, Network::Gateway& gateway);

   private:
    void SendTransformMessage(common::TransformMessageTypes type, common::TransformAction action);
    bool FindState(common::TransformMessageTypes type);
    void SubscribeForPushActions();
    void SubscribeForPopActions();

   private:
    MrpgGameContext& gameContext_;
    Input::InputManager* inputManager_;
    Network::Gateway& gateway_;
    std::list<common::TransformMessageTypes> states_;
};

typedef std::unique_ptr<PlayerController> PlayerControllerPtr;
}  // namespace MmmoRpg
