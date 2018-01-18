#pragma once
#include "Gateway.h"
#include "User.h"
#include "Manager.h"
#include "Context.h"
#include "Database/IDatabaseWrapper.h"
#include "Scenarios/ScenariosTypes.h"
#include "Scenarios/IScenario.h"
#include <unordered_map>

namespace GameServer
{
	typedef std::unordered_map<Scenario::ScenariosTypes, Scenario::ScenarioPtr> Scenarios;

	class GameServer
	{
	public:
		GameServer();
		void Update();

	private:
		void OnMessageArrived(const Network::BoxMessage& mesage);
		void NewUser(const std::string& name, uint32 id);
		void ProccesSdlEvent();

	private:
		bool running_;
		Manager manager_;

		std::mutex usersMutex;
		std::unordered_map<uint32, User> users;

		Network::GatewayPtr gateway_;
		Context context_;
	};
} // GameServer
