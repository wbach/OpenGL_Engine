#pragma once
#include "Common/MessageHandling/AbstractHandler.h"

class CGameObject;

namespace Network
{
	struct GetCharacterDataMsgResp;
} // Network

namespace MmmoRpg
{
	class NetworkCharacterManager;	

	class GetCharacterDataHandler : public common::AbstractHandler
	{
		typedef std::function<void(uint32)> Callback;
		typedef std::function<void(CGameObject*, const vec3&)> AddGameObject;

	public:
		GetCharacterDataHandler(NetworkCharacterManager& networkCharacterManager)
			: common::AbstractHandler({ Network::MessageTypes::GetCharacterDataResp })
			, networkCharacterManager_(networkCharacterManager)
		{
		}
	private:
		void HandleNetworkCharacterMsg(const Network::GetCharacterDataMsgResp& data);

	protected:		
		virtual void ProcessMessage(const Network::BoxMessage& message) override;
		NetworkCharacterManager&  networkCharacterManager_;
	};
} // MmmoRpg
