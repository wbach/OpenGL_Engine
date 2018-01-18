#pragma once
#include "Types.h"
#include <vector>
#include <memory>
#include <functional>

namespace GameServer
{
	namespace Scenario
	{
		typedef std::function<void()> Action;

		enum class ActionStatus
		{
			NONE,
			WORKING,
			DONE
		};

		class IScenario
		{
		public:
			virtual ~IScenario() {}
			virtual ActionStatus Perform() = 0;

		private:
			std::vector<Action> actions_;
		};

		typedef std::shared_ptr<Scenario::IScenario> ScenarioPtr;
	} // Scenario
} // GameServer
