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
	struct MrpgGameContext;

	class GetCharacterDataHandler : public common::AbstractHandler
	{
		typedef std::function<void(uint32)> Callback;
		typedef std::function<void(CGameObject*, const vec3&)> AddGameObject;

	public:
		GetCharacterDataHandler(NetworkCharacterManager& networkCharacterManager, MrpgGameContext& gameContext)
			: common::AbstractHandler({ Network::MessageTypes::GetCharacterDataResp })
			, networkCharacterManager_(networkCharacterManager)
			, gameContext_(gameContext)
		{
		}
	private:
		void HandleNetworkCharacterMsg(const Network::GetCharacterDataMsgResp& data);

	protected:
		virtual void ProcessMessage(const Network::BoxMessage& message) override;
		NetworkCharacterManager&  networkCharacterManager_;
		MrpgGameContext& gameContext_;
	};
} // MmmoRpg
