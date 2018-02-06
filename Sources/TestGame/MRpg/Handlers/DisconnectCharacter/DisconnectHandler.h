#pragma once
#include "Common/MessageHandling/AbstractHandler.h"
#include "Messages/GetCharacters/CharacterInfo.h"

namespace MmmoRpg
{
	class NetworkCharacterManager;

	class DisconnectHandler : public common::AbstractHandler
	{
	public:
		DisconnectHandler(NetworkCharacterManager& networkCharacterManager)
			: common::AbstractHandler({ Network::MessageTypes::DisconnectCharacter })
			, networkCharacterManager_(networkCharacterManager)
		{
		}

	protected:
		virtual void ProcessMessage(const Network::BoxMessage& message) override;
		NetworkCharacterManager&  networkCharacterManager_;
	};
} // MmmoRpg
