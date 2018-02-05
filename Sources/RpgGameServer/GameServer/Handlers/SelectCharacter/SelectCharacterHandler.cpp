#include "SelectCharacterHandler.h"
#include "Messages/SelectCharacter/SelectCharacterMsgReq.h"
#include "Messages/SelectCharacter/SelectCharacterMsgResp.h"

namespace GameServer
{
	namespace Handler
	{
		void SelectCharacterHandler::ProcessMessage(const Network::BoxMessage & message)
		{
			auto msg = CastToMsgType<Network::SelectCharacterMsgReq>(message.second);

			if (!msg) return;

			auto userId = message.first;
			auto characterId = msg->id;			

			auto& user = context_.GetUser(userId);			
			bool hasChar = user.HasCharacter(characterId);

			uint32 mapId = 0;
			
			if(hasChar)
				mapId = context_.databaseWrapper_->GetCharacterData(characterId).value().mapId;

			SendResponse(hasChar, userId, characterId, mapId);

			if (!hasChar)
			{
				Log("SelectCharacterHandler::ProcessMessage user " + std::to_string(userId) + " dont have character : " + std::to_string(characterId));
				return;
			}

			context_.manager_.AddHero(characterId);
			user.SetUsageCharacter(characterId);

			//user.GetUsageCharacterId

		}
		void SelectCharacterHandler::SendResponse(bool status, uint32 userId, uint32 characterId, uint32 mapId)
		{
			auto resp = std::make_unique<Network::SelectCharacterMsgResp>();
			resp->status_ = status ? Network::MessageStatus::Ok : Network::MessageStatus::Fail;
			resp->id = characterId;
			resp->mapId = mapId;
			context_.sendMessage_(userId, resp.get());
		}
	} // Handler
} // GameServer