#include <GameEngine/Components/Controllers/AI/IntentStateMachine/IntentStateEvents.h>
#include <GameEngine/Components/Controllers/AI/IntentStateMachine/IntentStateMachine.h>
#include <GameEngine/Components/Controllers/AI/ReactionStateMachine/ReactionAIEvents.h>
#include <gtest/gtest.h>

using namespace GameEngine::Components;

struct IntentStateMachineTests : public ::testing::Test
{
    IntentStateMachine sut_{RoutineState{nullptr}, ReactionState{}, QuestState{}, DeadState{}};

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

TEST_F(IntentStateMachineTests, Routine_DeathEvent_GoesToDead)
{
    handle(DeathEvent{});
    expectState<DeadState>();
}

TEST_F(IntentStateMachineTests, Routine_ReactionAIEvent_GoesToReaction)
{
    handle(ReactionAIEvent{NoiseHeard{}});
    expectState<ReactionState>();
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

TEST_F(IntentStateMachineTests, Quest_DeathEvent_GoesToDead)
{
    handle(QuestTriggered{});
    handle(DeathEvent{});
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
    handle(ThreatEnded{});
    handle(TargetLost{});

    expectState<ReactionState>();
}

TEST_F(IntentStateMachineTests, Dead_IsTerminal)
{
    handle(DeathEvent{});

    handle(QuestTriggered{});
    handle(QuestEnded{});
    handle(ReactionAIEvent{TargetSpotted{}});
    handle(ThreatEnded{});

    expectState<DeadState>();
}
