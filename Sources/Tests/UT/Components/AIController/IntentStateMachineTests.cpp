#include <GameEngine/Components/Controllers/AI/IntentStateMachine/IntentStateEvents.h>
#include <GameEngine/Components/Controllers/AI/IntentStateMachine/IntentStateMachine.h>
#include <GameEngine/Components/Controllers/AI/ReactionStateMachine/ReactionAIEvents.h>
#include <GameEngine/Components/Controllers/AI/RoutineComponent.h>
#include <gtest/gtest.h>

#include "Tests/UT/Components/BaseComponent.h"

using namespace GameEngine;
using namespace GameEngine::Components;

struct IntentStateMachineTests : public BaseComponentTestSchould
{
    RoutineComponent routineComponent_;
    IntentStateMachine sut_;

    IntentStateMachineTests()
        : BaseComponentTestSchould()
        , routineComponent_(context_, *obj_)
        , sut_(RoutineState(&routineComponent_), ReactionState{}, QuestState{}, DeadState{})
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
};

TEST_F(IntentStateMachineTests, StartsInRoutineState)
{
    expectState<RoutineState>();
}

TEST_F(IntentStateMachineTests, Routine_QuestTriggered_GoesToQuest)
{
    handle(QuestTriggered{});
    expectState<QuestState>();
}

TEST_F(IntentStateMachineTests, Routine_QuestTriggered_GoesToReactionDruringQuest)
{
    handle(QuestTriggered{});
    expectState<QuestState>();
    EXPECT_TRUE(routineComponent_.IsRoutineStateOfType<SuspendedState>());
    handle(ThreatEnded{});
    expectState<QuestState>();
}

TEST_F(IntentStateMachineTests, Routine_DeathEvent_GoesToDead)
{
    handle(EntityDiedEvent{});
    expectState<DeadState>();
}

TEST_F(IntentStateMachineTests, Routine_ReactionAIEvent_GoesToReaction)
{
    handle(ReactionAIEvent{NoiseHeard{}});
    expectState<ReactionState>();
    EXPECT_TRUE(routineComponent_.IsRoutineStateOfType<SuspendedState>());
    handle(ThreatEnded{});
    expectState<RoutineState>();
}

TEST_F(IntentStateMachineTests, Routine_UnrelatedEvents_StayInRoutine)
{
    handle(QuestEnded{});
    handle(ThreatEnded{});
    handle(NoiseHeard{});
    expectState<RoutineState>();
}

TEST_F(IntentStateMachineTests, Quest_QuestEnded_GoesToRoutine)
{
    handle(QuestTriggered{});
    handle(QuestEnded{});
    expectState<RoutineState>();
}

TEST_F(IntentStateMachineTests, Quest_EntityDiedEvent_GoesToDead)
{
    handle(QuestTriggered{});
    handle(EntityDiedEvent{});
    expectState<DeadState>();
}

TEST_F(IntentStateMachineTests, Quest_ReactionAIEvent_GoesToReaction)
{
    handle(QuestTriggered{});
    handle(ReactionAIEvent{DamageTaken{}});
    expectState<ReactionState>();
}

TEST_F(IntentStateMachineTests, Reaction_NoIntentTransitionsYet)
{
    handle(ReactionAIEvent{TargetSpotted{}});

    handle(QuestTriggered{});
    handle(QuestEnded{});
    handle(TargetLost{});

    expectState<ReactionState>();
}

TEST_F(IntentStateMachineTests, Dead_IsTerminal)
{
    handle(EntityDiedEvent{});

    handle(QuestTriggered{});
    handle(QuestEnded{});
    handle(ReactionAIEvent{TargetSpotted{}});
    handle(ThreatEnded{});

    expectState<DeadState>();
}
