#include <GameEngine/Components/Controllers/AI/AIController.h>
#include <GameEngine/Components/Controllers/AI/Professions/ScoutProfessionComponent.h>
#include <GameEngine/Components/Controllers/AI/ReactionStateMachine/States/CombatState/CombatState.h>
#include <GameEngine/Components/Controllers/AI/RoutineComponent.h>
#include <GameEngine/Components/Controllers/CharacterController/CharacterController.h>
#include <GameEngine/Components/Gameplay/CharacterStats/CharacterStatsComponent.h>
#include <GameEngine/Components/Gameplay/HealthComponent.h>
#include <GameEngine/Components/Physics/CapsuleShape.h>
#include <GameEngine/Components/Physics/Rigidbody.h>
#include <GameEngine/Time/DayNightCycle.h>

#include <GameEngine/Components/Controllers/AI/AIControllerImpl.inl>

#include "Tests/UT/EngineBasedTest.h"

using namespace GameEngine;
using namespace GameEngine::Components;
using namespace ::testing;

namespace
{
static constexpr int MINUTES_PER_DAY = 24 * 60;
const GameEngine::GameTime::HourMinuteSecond SLEEP_START{22, 0};
const GameEngine::GameTime::HourMinuteSecond WORK_START{6, 0};
}  // namespace

struct AIControllerTests : public EngineBasedTest
{
    void SimulateTimeSteps(int steps)
    {
        LOG_DEBUG << "Steps : " << steps;
        PrintCurrentTime();
        for (int i = 0; i < steps; ++i)
        {
            LOG_DEBUG << "deltaTime: " << deltaTime;
            scene->FullUpdate(deltaTime);
        }
        PrintCurrentTime();
    }

    void AdvanceMinutes(int minutes)
    {
        if (minutes > 0)
        {
            SimulateTimeSteps(minutes);
        }
    }
    void AdvanceToTime(const GameEngine::GameTime::HourMinuteSecond& time)
    {
        LOG_DEBUG << scene->GetDayNightCycle() << ", Target time: " << time;
        auto diff = CalculateStepsToTime(time.hour, time.minute);
        if (diff > 0)
        {
            const auto lastDeltaTime = deltaTime;
            deltaTime                = diff * 1000;
            SimulateTimeSteps(1);
            deltaTime = lastDeltaTime;
        }
        PrintCurrentTime();
    }

    void SimulateTimeStepsToTime(int targetHour, int targetMinute)
    {
        auto diff = CalculateStepsToTime(targetHour, targetMinute);
        if (diff > 0)
        {
            SimulateTimeSteps(diff);
        }
    }

    int CalculateStepsToTime(int targetHour, int targetMinute)
    {
        const auto hms     = scene->GetDayNightCycle().GetHourMinuteSecond();
        const auto current = hms.hour * 60 + hms.minute;
        const auto target  = ((targetHour * 60 + targetMinute) % MINUTES_PER_DAY + MINUTES_PER_DAY) % MINUTES_PER_DAY;
        const auto diff    = (target - current + MINUTES_PER_DAY) % MINUTES_PER_DAY;
        return diff;
    }

    void PrintCurrentTime()
    {
        LOG_DEBUG << scene->GetDayNightCycle();
    }

    void SetHour(const GameTime::HourMinuteSecond& hour)
    {
        auto& dnc = scene->GetDayNightCycle();
        dnc.SetHourMinuteSecond(hour);
        PrintCurrentTime();
    }

    void PrepareSut()
    {
        auto gameObject = scene->CreateGameObject("SUT");

        gameObject->AddComponent<CharacterController>();
        gameObject->AddComponent<Rigidbody>();
        gameObject->AddComponent<CapsuleShape>();

        stats_      = &gameObject->AddComponent<CharacterStatsComponent>();
        routine_    = &gameObject->AddComponent<RoutineComponent>();
        profession_ = &gameObject->AddComponent<ScoutProfessionComponent>();
        sut_        = &gameObject->AddComponent<AIController>();

        scene->AddGameObject(std::move(gameObject));
        scene->ProcessEvents();

        routine_->AddStep(RoutineStep{
            .startHour = SLEEP_START.hour, .startMinute = SLEEP_START.minute, .behaviorState = AIBehaviorState::Sleep});
        routine_->AddStep(
            RoutineStep{.startHour = WORK_START.hour, .startMinute = WORK_START.minute, .behaviorState = AIBehaviorState::Work});
        routine_->InitializeStateMachine();

        auto& dnc = scene->GetDayNightCycle();
        dnc.SetDayLengthInSeconds(MINUTES_PER_DAY);
        scene->Start();

        const auto& impl = sut_->getImpl();
        reactionState    = &impl.intentStateMachine_->getState<ReactionState>();

        EXPECT_TRUE(routine_->IsRoutineStateOfType<WorkState>());
        EXPECT_TRUE(reactionState->isCurrentStateOfType<PassiveState>());
    }

    void SetUp() override
    {
        EngineBasedTest::SetUp();
        PrepareSut();
        PrintCurrentTime();
        LOG_DEBUG << "Test case started.";
    }

    AIController* sut_{nullptr};
    CharacterStatsComponent* stats_{nullptr};
    RoutineComponent* routine_{nullptr};
    ReactionState* reactionState{nullptr};
    ScoutProfessionComponent* profession_{nullptr};
    float deltaTime{1000};
};

TEST_F(AIControllerTests, Routine_SleepToWork)
{
    SetHour(GameTime::HourMinuteSecond{.hour = 21, .minute = 59, .second = 59});

    ASSERT_EQ(routine_->GetCurrentBehaviorState(), AIBehaviorState::Work);

    AdvanceToTime(SLEEP_START);
    ASSERT_EQ(routine_->GetCurrentBehaviorState(), AIBehaviorState::Sleep);

    AdvanceToTime(WORK_START);
    ASSERT_EQ(routine_->GetCurrentBehaviorState(), AIBehaviorState::Work);
}

TEST_F(AIControllerTests, FullDayCycle_AllFourStates)
{
    routine_->AddStep(RoutineStep{.startHour = 12, .startMinute = 0, .behaviorState = AIBehaviorState::Eat});
    routine_->AddStep(RoutineStep{.startHour = 14, .startMinute = 0, .behaviorState = AIBehaviorState::Rest});

    SetHour(GameTime::HourMinuteSecond{.hour = 21, .minute = 59, .second = 59});
    ASSERT_EQ(routine_->GetCurrentBehaviorState(), AIBehaviorState::Work);

    AdvanceToTime(GameTime::HourMinuteSecond{.hour = 22, .minute = 0});
    ASSERT_EQ(routine_->GetCurrentBehaviorState(), AIBehaviorState::Sleep);

    AdvanceToTime(GameTime::HourMinuteSecond{.hour = 6, .minute = 0});
    ASSERT_EQ(routine_->GetCurrentBehaviorState(), AIBehaviorState::Work);

    AdvanceToTime(GameTime::HourMinuteSecond{.hour = 12, .minute = 0});
    ASSERT_EQ(routine_->GetCurrentBehaviorState(), AIBehaviorState::Eat);

    LOG_DEBUG << "1";
    AdvanceToTime(GameTime::HourMinuteSecond{.hour = 14, .minute = 0});
    ASSERT_EQ(routine_->GetCurrentBehaviorState(), AIBehaviorState::Rest);
    LOG_DEBUG << "2";

    AdvanceToTime(GameTime::HourMinuteSecond{.hour = 22, .minute = 0});
    LOG_DEBUG << "3";
    ASSERT_EQ(routine_->GetCurrentBehaviorState(), AIBehaviorState::Sleep);
}

TEST_F(AIControllerTests, Jump_DailyCycleContinuity)
{
    routine_->AddStep(RoutineStep{.startHour = 12, .startMinute = 0, .behaviorState = AIBehaviorState::Eat});
    routine_->AddStep(RoutineStep{.startHour = 14, .startMinute = 0, .behaviorState = AIBehaviorState::Rest});

    SetHour(GameTime::HourMinuteSecond{.hour = 21, .minute = 59, .second = 59});
    ASSERT_EQ(routine_->GetCurrentBehaviorState(), AIBehaviorState::Work);
    EXPECT_TRUE(routine_->IsRoutineStateOfType<WorkState>());

    AdvanceToTime(GameTime::HourMinuteSecond{.hour = 22, .minute = 0});
    ASSERT_EQ(routine_->GetCurrentBehaviorState(), AIBehaviorState::Sleep);
    EXPECT_TRUE(routine_->IsRoutineStateOfType<SleepState>());

    AdvanceToTime(GameTime::HourMinuteSecond{.hour = 14, .minute = 0});
    ASSERT_EQ(routine_->GetCurrentBehaviorState(), AIBehaviorState::Rest);
    EXPECT_TRUE(routine_->IsRoutineStateOfType<RestState>());
}

TEST_F(AIControllerTests, InterruptRoutineByDamageTakenEventAndBackWhenTargetLost)
{
    routine_->AddStep(RoutineStep{.startHour = 12, .startMinute = 0, .behaviorState = AIBehaviorState::Eat});
    routine_->AddStep(RoutineStep{.startHour = 14, .startMinute = 0, .behaviorState = AIBehaviorState::Rest});

    SetHour(GameTime::HourMinuteSecond{.hour = 20, .minute = 13, .second = 59});
    SimulateTimeSteps(1);

    ASSERT_EQ(routine_->GetCurrentBehaviorState(), AIBehaviorState::Rest);
    EXPECT_TRUE(routine_->IsRoutineStateOfType<RestState>());

    auto gameObject    = scene->CreateGameObject("Enemy");
    auto gameObjectPtr = gameObject.get();
    scene->AddGameObject(std::move(gameObject));
    scene->ProcessEvents();

    sut_->pushEventToQueue(DamageTaken{.target = gameObjectPtr});
    SimulateTimeSteps(1);

    EXPECT_TRUE(routine_->IsRoutineStateOfType<SuspendedState>());
    EXPECT_TRUE(reactionState->isCurrentStateOfType<CombatState>());

    sut_->pushEventToQueue(TargetLost{});
    SimulateTimeSteps(1);

    EXPECT_TRUE(routine_->IsRoutineStateOfType<RestState>());
    EXPECT_TRUE(reactionState->isCurrentStateOfType<PassiveState>());
}
