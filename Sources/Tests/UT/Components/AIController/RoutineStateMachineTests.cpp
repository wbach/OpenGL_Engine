#include <GameEngine/Components/Controllers/AI/ProfessionComponent.h>
#include <GameEngine/Components/Controllers/AI/Professions/ScoutProfessionComponent.h>
#include <GameEngine/Components/Controllers/AI/RoutineStateMachine/RoutineAIEvents.h>
#include <GameEngine/Components/Controllers/AI/RoutineStateMachine/RoutineContext.h>
#include <GameEngine/Components/Controllers/AI/RoutineStateMachine/RoutineStateMachine.h>
#include <GameEngine/Objects/GameObject.h>
#include <gtest/gtest.h>

#include "Tests/UT/Components/BaseComponent.h"

using namespace GameEngine;
using namespace GameEngine::Components;

struct RoutineStateMachineTests : public BaseComponentTestSchould
{
    RoutineStateMachineTests()
        : BaseComponentTestSchould()
        , profession(context_, *obj_)
        , routineContext{&profession, *obj_}
        , sut_(WorkState{routineContext}, RestState{}, EatState{}, SleepState{}, SuspendedState{})
    {
    }

    template <typename State>
    void expectState()
    {
        EXPECT_TRUE(sut_.isCurrentStateOfType<State>()) << "Current state: " << sut_.getCurrentStateName();
    }

    template <typename Event>
    void handle(const Event& event)
    {
        sut_.handle(event);
    }

    ScoutProfessionComponent profession;
    RoutineContext routineContext;
    RoutineStateMachine sut_;
};

TEST_F(RoutineStateMachineTests, StartsInWorkState)
{
    expectState<WorkState>();
}

TEST_F(RoutineStateMachineTests, WorkState_WorkStarted_StaysInWorkState)
{
    handle(WorkStarted{});
    expectState<WorkState>();
}

TEST_F(RoutineStateMachineTests, WorkState_MealStarted_GoesToEatState)
{
    handle(MealStarted{});
    expectState<EatState>();
}

TEST_F(RoutineStateMachineTests, WorkState_SleepStarted_GoesToSleepState)
{
    handle(SleepStarted{});
    expectState<SleepState>();
}

TEST_F(RoutineStateMachineTests, WorkState_Suspend_GoesToSuspendedState)
{
    handle(Suspend{});
    expectState<SuspendedState>();
}

TEST_F(RoutineStateMachineTests, WorkState_Resume_StaysInWorkState)
{
    handle(Resume{});
    expectState<WorkState>();
}

TEST_F(RoutineStateMachineTests, WorkState_RestStarted_GoesToRestState)
{
    handle(RestStarted{});
    expectState<RestState>();
}

TEST_F(RoutineStateMachineTests, EatState_WorkStarted_GoesToWorkState)
{
    handle(MealStarted{});
    expectState<EatState>();
    handle(WorkStarted{});
    expectState<WorkState>();
}

TEST_F(RoutineStateMachineTests, EatState_SleepStarted_GoesToSleepState)
{
    handle(MealStarted{});
    expectState<EatState>();
    handle(SleepStarted{});
    expectState<SleepState>();
}

TEST_F(RoutineStateMachineTests, EatState_Suspend_GoesToSuspendedState)
{
    handle(MealStarted{});
    expectState<EatState>();
    handle(Suspend{});
    expectState<SuspendedState>();
}

TEST_F(RoutineStateMachineTests, EatState_Resume_StaysInEatState)
{
    handle(MealStarted{});
    expectState<EatState>();
    handle(Resume{});
    expectState<EatState>();
}

TEST_F(RoutineStateMachineTests, EatState_RestStarted_GoesToRestState)
{
    handle(MealStarted{});
    expectState<EatState>();
    handle(RestStarted{});
    expectState<RestState>();
}

TEST_F(RoutineStateMachineTests, RestState_WorkStarted_GoesToWorkState)
{
    handle(RestStarted{});
    expectState<RestState>();
    handle(WorkStarted{});
    expectState<WorkState>();
}

TEST_F(RoutineStateMachineTests, RestState_MealStarted_GoesToEatState)
{
    handle(RestStarted{});
    expectState<RestState>();
    handle(MealStarted{});
    expectState<EatState>();
}

TEST_F(RoutineStateMachineTests, RestState_SleepStarted_GoesToSleepState)
{
    handle(RestStarted{});
    expectState<RestState>();
    handle(SleepStarted{});
    expectState<SleepState>();
}

TEST_F(RoutineStateMachineTests, RestState_Suspend_GoesToSuspendedState)
{
    handle(RestStarted{});
    expectState<RestState>();
    handle(Suspend{});
    expectState<SuspendedState>();
}

TEST_F(RoutineStateMachineTests, RestState_Resume_StaysInRestState)
{
    handle(RestStarted{});
    expectState<RestState>();
    handle(Resume{});
    expectState<RestState>();
}

TEST_F(RoutineStateMachineTests, RestState_RestStarted_StaysInRestState)
{
    handle(RestStarted{});
    expectState<RestState>();
    handle(RestStarted{});
    expectState<RestState>();
}

TEST_F(RoutineStateMachineTests, SleepState_WorkStarted_GoesToWorkState)
{
    handle(SleepStarted{});
    expectState<SleepState>();
    handle(WorkStarted{});
    expectState<WorkState>();
}

TEST_F(RoutineStateMachineTests, SleepState_MealStarted_GoesToEatState)
{
    handle(SleepStarted{});
    expectState<SleepState>();
    handle(MealStarted{});
    expectState<EatState>();
}

TEST_F(RoutineStateMachineTests, SleepState_Suspend_GoesToSuspendedState)
{
    handle(SleepStarted{});
    expectState<SleepState>();
    handle(Suspend{});
    expectState<SuspendedState>();
}

TEST_F(RoutineStateMachineTests, SleepState_Resume_StaysInSleepState)
{
    handle(SleepStarted{});
    expectState<SleepState>();
    handle(Resume{});
    expectState<SleepState>();
}

TEST_F(RoutineStateMachineTests, SleepState_RestStarted_GoesToRestState)
{
    handle(SleepStarted{});
    expectState<SleepState>();
    handle(RestStarted{});
    expectState<RestState>();
}

TEST_F(RoutineStateMachineTests, SuspendedState_WorkStarted_GoesToWorkState)
{
    handle(Suspend{});
    expectState<SuspendedState>();
    handle(WorkStarted{});
    expectState<WorkState>();
}

TEST_F(RoutineStateMachineTests, SuspendedState_MealStarted_GoesToEatState)
{
    handle(Suspend{});
    expectState<SuspendedState>();
    handle(MealStarted{});
    expectState<EatState>();
}

TEST_F(RoutineStateMachineTests, SuspendedState_SleepStarted_GoesToSleepState)
{
    handle(Suspend{});
    expectState<SuspendedState>();
    handle(SleepStarted{});
    expectState<SleepState>();
}

TEST_F(RoutineStateMachineTests, SuspendedState_Resume_GoesToWorkState)
{
    handle(Suspend{});
    expectState<SuspendedState>();
    handle(Resume{});
    expectState<WorkState>();
}

TEST_F(RoutineStateMachineTests, SuspendedState_RestStarted_GoesToRestState)
{
    handle(Suspend{});
    expectState<SuspendedState>();
    handle(RestStarted{});
    expectState<RestState>();
}
