#include "GetCharactersDataHandler.h"
#include "Common/Messages/GetCharacterData/GetCharacterDataMsgReq.h"
#include "Common/Messages/GetCharacterData/GetCharacterDataMsgResp.h"
#include "Common/Messages/GetCharacterData/GetCharactersDataMsgReq.h"

namespace GameServer
{
namespace Handler
{
void GetCharactersDataHandler::ProcessMessage(Network::UserId userId, const Network::IMessage& message)
{
    if (message.GetType() == common::MessageTypes::GetCharactersDataReq)
        ProccesGetCharactersDataReq(message);

    if (message.GetType() == common::MessageTypes::GetCharacterDataReq)
        ProccesGetCharacterDataReq(userId, message);
}
void GetCharactersDataHandler::ProccesGetCharactersDataReq(const Network::IMessage& message)
{
    auto msg = static_cast<const common::GetCharactersDataMsgReq*>(&message);

    auto charactersOnMap = context_.manager_.GetAllCharactersInMap(msg->mapId);

    for (const auto& c : charactersOnMap)
    {
        auto resp               = std::make_unique<common::GetCharacterDataMsgResp>();
        auto& characterContext  = c.second->context_;
        resp->networkCharcterId = c.first;
        resp->characterData     = characterContext.data_;
        resp->commonStats       = characterContext.commonStats_;
        resp->position          = characterContext.transform_.GetPosition();
        resp->rotation          = characterContext.transform_.GetRotation();

        for (const auto& user : context_.GetUsers())  // TO DO : all character to new user, but only new character to other users
            context_.sendMessage_(user.first, *resp);
    }
}
void GetCharactersDataHandler::ProccesGetCharacterDataReq(Network::UserId userId, const Network::IMessage& message)
{
    auto msg = static_cast<const common::GetCharacterDataMsgReq*>(&message);

    auto hero = context_.manager_.GetHero(msg->networkCharacterId);

    if (hero == nullptr)
        return;

    auto resp               = std::make_unique<common::GetCharacterDataMsgResp>();
    auto& characterContext  = hero->context_;
    resp->networkCharcterId = msg->networkCharacterId;
    resp->characterData     = characterContext.data_;
    resp->commonStats       = characterContext.commonStats_;
    resp->position          = characterContext.transform_.GetPosition();
    resp->rotation          = characterContext.transform_.GetRotation();

    context_.sendMessage_(userId, *resp);
}
}  // namespace Handler
}  // namespace GameServer
