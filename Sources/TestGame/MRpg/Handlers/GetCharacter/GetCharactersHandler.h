#pragma once
#include "Common/MessageHandling/AbstractHandler.h"
#include "Messages/GetCharacters/CharacterInfo.h"

namespace MmmoRpg
{
	class GetCharactersHandler : public common::AbstractHandler
	{
		typedef std::function<void(const std::vector<Network::CharacterInfo>&)> GetCharacterFunc;
	public:

		GetCharactersHandler(GetCharacterFunc characters_)
			: common::AbstractHandler({ Network::MessageTypes::GetCharactersResp })
			, getCharacter_(characters_)
		{
		}

	protected:
		virtual void ProcessMessage(const Network::BoxMessage& message) override;
		GetCharacterFunc getCharacter_;
	};
} // MmmoRpg
