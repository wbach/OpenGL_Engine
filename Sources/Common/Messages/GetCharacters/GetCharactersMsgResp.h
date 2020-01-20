#pragma once
#include "CharacterInfo.h"
#include "../IMessage.h"
#include "../MessageTarget.h"
#include "optional.hpp"

namespace Network
{
	const uint8 charactersPerAcount = 5;
	struct GetCharactersMsgResp : public IMessage
	{
		GetCharactersMsgResp()
			: IMessage(MessageTypes::GetCharactersResp, MessageTarget::Dedicated)
		{}

		wb::optional<CharacterInfo> characterInfo[charactersPerAcount];

		virtual std::string ToString() override
		{
			std::string result = "GetCharactersMsgResp characters: {";

			for(const auto& ci : characterInfo)
			{
				if(!ci)	continue;

				result += "\nId : " + std::to_string(ci.constValue().id_);
			}

			return result + "\n}";
		}
	};
} // Network
