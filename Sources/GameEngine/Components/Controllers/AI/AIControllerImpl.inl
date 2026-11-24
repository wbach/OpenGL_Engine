#pragma once

#include <Blackboard.h>

#include "AIController.h"
#include "AIControllerContext.h"
#include "ActionStateMachine/ActionStateMachine.h"
#include "IntentStateMachine/IntentContext.h"
#include "IntentStateMachine/IntentStateMachine.h"

namespace GameEngine
{
namespace Components
{
struct AIController::Impl
{
    std::unique_ptr<ActionStateMachine> stateMachine_;
    std::unique_ptr<IntentStateMachine> intentStateMachine_;
    std::unique_ptr<AIControllerContext> controllerContext_;
    std::unique_ptr<IntentContext> intentContext_;

    Utils::Blackboard blackboard_;

    void CleanUp()
    {
        stateMachine_.reset();
    }
};
}  // namespace Components
}  // namespace GameEngine
