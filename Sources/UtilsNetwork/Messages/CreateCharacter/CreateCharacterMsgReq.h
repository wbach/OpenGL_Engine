#pragma once
#include "../IMessage.h"
#include "../MessageTarget.h"

namespace Network
{
	class CreateCharacterMsgReq : public IMessage
	{
	public:
		CreateCharacterMsgReq(const std::string& name)
			: IMessage(MessageTypes::CreateCharacterReq, MessageTarget::Dedicated)
		{
			memset(characterName_, 0, arraySize);
			CopyToArray(characterName_, name);
		}
		virtual std::string ToString() override
		{
			return "Selected character ID: " + std::to_string(id);
		}
		std::string GetCharacterName()
		{
			return characterName_;
		}
	public:
		uint32 id = 0;

	private:
		char characterName_[arraySize];
	};
} // Network
