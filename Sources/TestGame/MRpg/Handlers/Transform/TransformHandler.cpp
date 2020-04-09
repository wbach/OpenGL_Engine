#include "TransformHandler.h"
#include <Common/Messages/TransformMessages/TransformMsgResp.h>
#include "Common/Controllers/CharacterController/Character.h"
#include "Common/Controllers/CharacterController/NetworkActionsConverter.h"
#include "TestGame/MRpg/Characters/NetworkCharacterManger.h"

using namespace common::Controllers;

namespace MmmoRpg
{
void TransformHandler::ProcessMessage(Network::UserId, const Network::IMessage& message)
{
    auto msg = static_cast<const common::TransformMsgResp*>(&message);

    if (msg == nullptr)
    {
        ERROR_LOG("Got msg but wrong type : " + std::to_string(message.GetType()));
        return;
    }
    HandleTransformMsg(*msg);
}

void TransformHandler::HandleTransformMsg(const common::TransformMsgResp& msg)
{
    auto networkCharacter = networkCharacterManager_.GetCharacter(msg.id);

    if (networkCharacter == nullptr)
        return;

    auto& gameObject = networkCharacter->GetGameObject();

    gameObject.worldTransform.SetPosition(msg.position);
    gameObject.worldTransform.SetRotation(DegreesVec3(msg.rotation));

    auto icontroller = networkCharacter->GetControllerByType(CharacterControllerType);
    auto controller  = castControllerAs<CharacterController>(icontroller);

    switch (msg.action)
    {
        case common::TransformAction::PUSH:
            controller->AddState(NetworkActionsConverter::Convert(msg.type));
            break;
        case common::TransformAction::POP:
            controller->RemoveState(NetworkActionsConverter::Convert(msg.type));
            break;
    }
}
}  // namespace MmmoRpg
