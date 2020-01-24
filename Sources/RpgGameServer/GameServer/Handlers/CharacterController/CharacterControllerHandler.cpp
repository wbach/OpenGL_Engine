#include "CharacterControllerHandler.h"
#include "Common/Controllers/CharacterController/Character.h"
#include "Common/Controllers/CharacterController/NetworkActionsConverter.h"
#include "Common/Messages/TransformMessages/TransformMsgReq.h"
#include "Common/Messages/TransformMessages/TransformMsgResp.h"

using namespace common::Controllers;

namespace GameServer
{
namespace Handler
{
CharacterControllerHandler::CharacterControllerHandler(Context& context)
    : common::AbstractHandler(common::MessageTypes::TransformReq)
    , context_(context)
{
}

void CharacterControllerHandler::ProcessMessage(uint32 userId, const Network::IMessage& message)
{
    auto msg = static_cast<const common::TransformMsgReq*>(&message);

    if (msg == nullptr)
        return;

    auto characterId = context_.GetUser(userId).GetUsageCharacterId();

    if (!characterId)
    {
        ERROR_LOG("CharacterControllerHandler::ProcessMessage receive message for not user who didnt select character yet.");
        return;
    }

    auto hero = context_.manager_.GetHero(characterId.value());

    if (hero == nullptr)
    {
        ERROR_LOG("CharacterControllerHandler::ProcessMessage cant get character by id.");
        return;
    }

    auto controller = hero->GetControllerByType(CharacterControllerType);

    if (controller == nullptr)
    {
        ERROR_LOG("CharacterControllerHandler::ProcessMessage character controller not found in character.");
        return;
    }

    auto characterController = common::Controllers::castControllerAs<CharacterController>(controller);

    switch (msg->action)
    {
        case common::TransformAction::PUSH:
            characterController->AddState(NetworkActionsConverter::Convert(msg->type));
            break;
        case common::TransformAction::POP:
            characterController->RemoveState(NetworkActionsConverter::Convert(msg->type));
            break;
    }

    auto tdata      = std::make_unique<common::TransformMsgResp>();
    tdata->position = characterController->GetTransform().GetPosition();
    tdata->rotation = characterController->GetTransform().GetRotation();
    tdata->id       = msg->id;
    tdata->type     = msg->type;
    tdata->action   = msg->action;

    for (auto& user : context_.GetUsers())
    {
        context_.sendMessage_(user.first, *tdata);
    }
}
}  // namespace Handler
}  // namespace GameServer
