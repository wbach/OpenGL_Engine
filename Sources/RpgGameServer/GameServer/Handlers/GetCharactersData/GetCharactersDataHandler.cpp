#include "GetCharactersDataHandler.h"
#include "Messages/GetCharacterData/GetCharacterDataMsgReq.h"
#include "Messages/GetCharacterData/GetCharactersDataMsgReq.h"
#include "Messages/GetCharacterData/GetCharacterDataMsgResp.h"

namespace GameServer
{
	namespace Handler
	{
		void GetCharactersDataHandler::ProcessMessage(const Network::BoxMessage& message)
		{
			if (message.second->GetType() == Network::MessageTypes::GetCharactersDataReq)
				ProccesGetCharactersDataReq(message);

			if (message.second->GetType() == Network::MessageTypes::GetCharacterDataReq)
				ProccesGetCharacterDataReq(message);
		}
		void GetCharactersDataHandler::ProccesGetCharactersDataReq(const Network::BoxMessage& message)
		{
			auto msg = Network::castMessageAs<Network::GetCharactersDataMsgReq>(message.second);

			auto charactersOnMap = context_.manager_.GetAllCharactersInMap(msg->mapId);

			for (const auto& c : charactersOnMap)
			{
				Network::GetCharacterDataMsgResp resp;
				auto& characterContext = c.second->context_;
				resp.networkCharcterId = c.first;
				resp.characterData = characterContext.data_;
				resp.commonStats = characterContext.commonStats_;
				resp.position = characterContext.transform_.GetPosition();
				resp.rotation = characterContext.transform_.GetRotation();
				context_.sendMessage_(message.first, Network::CreateIMessagePtr<Network::GetCharacterDataMsgResp>(resp));
			}
		}
		void GetCharactersDataHandler::ProccesGetCharacterDataReq(const Network::BoxMessage& message)
		{
			auto msg = Network::castMessageAs<Network::GetCharacterDataMsgReq>(message.second);

		}
	} // Handler
} // GameServer