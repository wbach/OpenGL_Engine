#pragma once
#include "../../../Common/Hero/Stats.h"
#include "../IMessage.h"
#include "../MessageTarget.h"
#include "optional.hpp"

namespace Network
{
	struct GetCharacterDataMsgResp : public IMessage
	{
		GetCharacterDataMsgResp()
			: IMessage(MessageTypes::GetCharacterDataResp, MessageTarget::Dedicated)
		{}

		uint32 networkCharcterId;
		common::Hero::CommonStats commonStats;
		common::Hero::CharacterData characterData;
		vec3 position;
		vec3 rotation;

		virtual std::string ToString() override
		{
			std::string result = "GetCharacterDataMsgResp{";
			return result + "\n}";
		}
	};
} // Network
