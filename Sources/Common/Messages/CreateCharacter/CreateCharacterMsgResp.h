#pragma once
#include "../IMessage.h"
#include "../MessageTarget.h"
#include "../MessageStatus.h"

namespace Network
{
	class CreateCharacterMsgResp: public IMessage
	{
	public:
		CreateCharacterMsgResp(const std::string& name)
			: IMessage(MessageTypes::CreateCharacterResp, MessageTarget::Dedicated)
		{
			memset(characterName_, 0, arraySize);
			CopyToArray(characterName_, name);
		}
		virtual std::string ToString() override
		{
			return "Created character ID: " + std::to_string(id) + " status : " + (status_ == MessageStatus::Ok ? "Ok" : "Fail");
		}
		std::string GetCharacterName()
		{
			return characterName_;
		}
	public:
		uint32 id = 0;
		MessageStatus status_;

	private:
		char characterName_[arraySize];
	};
} // Network
