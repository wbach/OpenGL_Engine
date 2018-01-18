#pragma once
#include "../IScenario.h"

namespace GameServer
{
	namespace Scenario
	{
		class CharacterSelect : public IScenario
		{
		public:
			CharacterSelect();
			virtual ActionStatus Perform() override;

		private:
			std::vector<Action> actions_;
		};

		typedef std::shared_ptr<Scenario::IScenario> ScenarioPtr;
	} // Scenario
} // GameServer
