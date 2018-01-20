#pragma once
#include "Gateway.h"
#include "Context.h"
#include "Dispatcher.h"

namespace GameServer
{
	class GameServer
	{
	public:
		GameServer();
		void Update();

	private:
		void OnMessageArrived(const Network::BoxMessage& mesage);
		
		void ProccesSdlEvent();

	private:
		bool running_;		
		Dispacher dispatcher_;

		Network::GatewayPtr gateway_;
		Context context_;
	};
} // GameServer
