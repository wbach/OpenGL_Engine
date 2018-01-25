#include "GetCharactersHandler.h"
#include "Messages/GetCharacters/GetCharactersMsgReq.h"
#include "Messages/GetCharacters/GetCharactersMsgResp.h"

namespace GameServer
{
	namespace Handler
	{
		void GetCharactersHandler::ProcessMessage(const Network::BoxMessage & message)
		{
			auto userId = message.first;
			auto& user = context_.GetUser(userId);
			auto userCharacters = user.GetCharacters();

			Network::GetCharactersMsgResp charactersMsgResp;

			uint8 i = 0;
			for(const auto& characterId : userCharacters)
			{
				if(i>=5) break;

				auto c = context_.databaseWrapper_->GetCharacterData(characterId);
				
				if(!c)
				{
					Log("GetCharactersHandler::ProcessMessage character not found " + std::to_string(characterId));
					continue;
				}

				auto& character = c.value();

				charactersMsgResp.characterInfo[i++] = Network::CharacterInfo(
													characterId, character.name,
													character.lvl,
													character.classId);
			}

			context_.sendMessage_(userId, Network::CreateIMessagePtr<Network::GetCharactersMsgResp>(charactersMsgResp));
		}
	} // Handler
} // GameServer