#pragma once
#include <functional>
#include <memory>
#include <vector>
#include "Types.h"

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
    virtual ~IScenario()
    {
    }
    virtual ActionStatus Perform() = 0;

   private:
    std::vector<Action> actions_;
};

typedef std::shared_ptr<Scenario::IScenario> ScenarioPtr;
}  // namespace Scenario
}  // namespace GameServer
