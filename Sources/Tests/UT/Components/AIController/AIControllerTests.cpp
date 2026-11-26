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
#include <Utils/Time/DeltaTime.h>

#include <GameEngine/Components/Controllers/AI/AIControllerImpl.inl>

#include "Tests/UT/EngineBasedTest.h"

using namespace GameEngine;
using namespace GameEngine::Components;
using namespace ::testing;

namespace
{
static constexpr int SECONDS_PER_DAY = 24 * 60 * 60;
const GameEngine::GameTime::HourMinuteSecond SLEEP_START{22, 0};
const GameEngine::GameTime::HourMinuteSecond WORK_START{6, 0};
}  // namespace

struct AIControllerTests : public EngineBasedTest
{
    void SimulateTimeSteps(int steps)
    {
        PrintCurrentTime();
        for (int i = 0; i < steps; ++i)
        {
            scene->FullUpdate(Utils::Time::DeltaTime(static_cast<int64>(deltaTime * Utils::Time::DeltaTime::NANOSECONDS_PER_SECOND)));
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
        auto diff = CalculateStepsToTime(time);
        if (diff > 0)
        {
            const auto lastDeltaTime = deltaTime;
            deltaTime                = diff * 1000;
            SimulateTimeSteps(1);
            deltaTime = lastDeltaTime;
        }
        PrintCurrentTime();
    }

    void SimulateTimeStepsToTime(const GameEngine::GameTime::HourMinuteSecond& time, AIBehaviorState expectedRoutineState)
    {
        const int steps = CalculateStepsToTime(time);

        for (int i = 0; i < steps; ++i)
        {
            scene->FullUpdate(Utils::Time::DeltaTime(static_cast<int64>(deltaTime * Utils::Time::DeltaTime::NANOSECONDS_PER_SECOND)));
            PrintCurrentTime();

            if (routine_->GetCurrentBehaviorState() != expectedRoutineState)
            {
                LOG_DEBUG << magic_enum::enum_name(routine_->GetCurrentBehaviorState())
                          << ", expected : " << magic_enum::enum_name(expectedRoutineState);
            }

            ASSERT_EQ(routine_->GetCurrentBehaviorState(), expectedRoutineState);
        }
    }

    int CalculateStepsToTime(const GameEngine::GameTime::HourMinuteSecond& time)
    {
        constexpr double SECONDS_PER_DAY = 86400.0;
        const auto hms                   = scene->GetDayNightCycle().GetHourMinuteSecond();
        const int current                = hms.hour * 3600 + hms.minute * 60 + hms.second;
        const int target = (time.hour * 3600 + time.minute * 60 + time.second) % static_cast<int>(SECONDS_PER_DAY);
        const int diff   = (target - current + static_cast<int>(SECONDS_PER_DAY)) % static_cast<int>(SECONDS_PER_DAY);

        if (diff == 0)
            return 0;

        if (deltaTime <= 0.f)
            throw std::runtime_error("deltaTime must be positive");

        return static_cast<int>(std::ceil(static_cast<double>(diff) / deltaTime));
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
        dnc.SetDayLengthInSeconds(SECONDS_PER_DAY);
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
    float deltaTime{1.f};
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

TEST_F(AIControllerTests, DISABLED_FullDayCycle_AllFourStates) //WIP
{
    routine_->ClearSteps();
    routine_->AddStep(RoutineStep{.startHour = 7, .startMinute = 0, .behaviorState = AIBehaviorState::Eat});
    routine_->AddStep(RoutineStep{.startHour = 7, .startMinute = 15, .behaviorState = AIBehaviorState::Work});
    routine_->AddStep(RoutineStep{.startHour = 12, .startMinute = 0, .behaviorState = AIBehaviorState::Eat});
    routine_->AddStep(RoutineStep{.startHour = 12, .startMinute = 15, .behaviorState = AIBehaviorState::Work});
    routine_->AddStep(RoutineStep{.startHour = 15, .startMinute = 0, .behaviorState = AIBehaviorState::Rest});
    routine_->AddStep(RoutineStep{.startHour = 21, .startMinute = 0, .behaviorState = AIBehaviorState::Sleep});

    SetHour(GameTime::HourMinuteSecond{.hour = 6, .minute = 0, .second = 0});
    SimulateTimeStepsToTime(GameTime::HourMinuteSecond{.hour = 6, .minute = 59, .second = 59}, AIBehaviorState::Sleep);
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
    routine_->AddStep(RoutineStep{.startHour = 13, .startMinute = 0, .behaviorState = AIBehaviorState::Eat});
    routine_->AddStep(RoutineStep{.startHour = 13, .startMinute = 15, .behaviorState = AIBehaviorState::Work});
    routine_->AddStep(RoutineStep{.startHour = 15, .startMinute = 0, .behaviorState = AIBehaviorState::Rest});

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
