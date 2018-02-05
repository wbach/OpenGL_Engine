#pragma once
#include "Common/MessageHandling/AbstractHandler.h"
#include "Messages/GetCharacters/CharacterInfo.h"

namespace MmmoRpg
{

	class SelectCharacterHandler : public common::AbstractHandler
	{
		typedef std::function<void(uint32, uint32, bool)> Callback;
	public:
		SelectCharacterHandler(Callback callback)
			: common::AbstractHandler({ Network::MessageTypes::SelectCharacterResp })
			, callback_(callback)
		{
		}

	protected:
		virtual void ProcessMessage(const Network::BoxMessage& message) override;
		Callback callback_;
	};
} // MmmoRpg
