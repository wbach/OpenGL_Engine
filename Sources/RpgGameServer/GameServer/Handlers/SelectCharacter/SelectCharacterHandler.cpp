#include "SelectCharacterHandler.h"
#include <Common/Messages/SelectCharacter/SelectCharacterMsgReq.h>
#include <Common/Messages/SelectCharacter/SelectCharacterMsgResp.h>

namespace GameServer
{
namespace Handler
{
void SelectCharacterHandler::ProcessMessage(Network::UserId userId, const Network::IMessage& message)
{
    auto msg = static_cast<const common::SelectCharacterMsgReq*>(&message);

    if (not msg)
        return;

    auto characterId = msg->id;

    auto& user   = context_.GetUser(userId);
    bool hasChar = user.HasCharacter(characterId);

    uint32 mapId = 0;

    if (hasChar)
    {
        mapId = context_.databaseWrapper_->GetCharacterData(characterId).value().mapId;
    }

    SendResponse(hasChar, userId, characterId, mapId);

    if (not hasChar)
    {
        DEBUG_LOG("SelectCharacterHandler::ProcessMessage user " + std::to_string(userId) + " dont have character : " + std::to_string(characterId));
        return;
    }

    context_.manager_.AddHero(characterId);
    user.SetUsageCharacter(characterId);
}
void SelectCharacterHandler::SendResponse(bool status, Network::UserId userId, uint32 characterId, uint32 mapId)
{
    auto resp     = std::make_unique<common::SelectCharacterMsgResp>();
    resp->status_ = status ? common::MessageStatus::Ok : common::MessageStatus::Fail;
    resp->id      = characterId;
    resp->mapId   = mapId;
    context_.sendMessage_(userId, *resp);
}
}  // namespace Handler
}  // namespace GameServer
