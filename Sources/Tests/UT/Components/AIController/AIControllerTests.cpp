#include <GameEngine/Components/Controllers/AI/AIController.h>
#include <GameEngine/Components/Controllers/AI/Professions/ScoutProfessionComponent.h>
#include <GameEngine/Components/Controllers/AI/RoutineComponent.h>
#include <GameEngine/Components/Controllers/CharacterController/CharacterController.h>
#include <GameEngine/Components/Gameplay/CharacterStats/CharacterStatsComponent.h>
#include <GameEngine/Components/Gameplay/HealthComponent.h>
#include <GameEngine/Time/DayNightCycle.h>

#include "Tests/UT/Components/BaseComponent.h"

using namespace GameEngine;
using namespace GameEngine::Components;
using namespace ::testing;

namespace
{
static constexpr int MINUTES_PER_DAY = 24 * 60;
const GameEngine::GameTime::HourMinuteSecond SLEEP_START{22, 0};
const GameEngine::GameTime::HourMinuteSecond WORK_START{6, 0};
}  // namespace

struct AIControllerTests : public BaseComponentTestSchould
{
    AIControllerTests()
        : BaseComponentTestSchould()
        , sut_(context_, *obj_)
    {
        obj_->AddComponent<CharacterController>();
        stats_      = &obj_->AddComponent<CharacterStatsComponent>();
        routine_    = &obj_->AddComponent<RoutineComponent>();
        profession_ = &obj_->AddComponent<ScoutProfessionComponent>();

        routine_->AddStep(RoutineStep{
            .startHour = SLEEP_START.hour, .startMinute = SLEEP_START.minute, .behaviorState = AIBehaviorState::Sleep});
        routine_->AddStep(
            RoutineStep{.startHour = WORK_START.hour, .startMinute = WORK_START.minute, .behaviorState = AIBehaviorState::Work});
        routine_->InitializeStateMachine();

        auto& dnc                = context_.scene_.GetDayNightCycle();
        context_.time_.deltaTime = 1000;
        dnc.SetDayLengthInSeconds(MINUTES_PER_DAY);
        sut_.ReqisterFunctions();

        componentController_.CallFunctions(FunctionType::Awake);
        componentController_.CallFunctions(FunctionType::LateAwake);
        componentController_.CallFunctions(FunctionType::OnStart);
    }

    void SimulateTimeSteps(int steps)
    {
        LOG_DEBUG << "Steps : " << steps;
        PrintCurrentTime();
        auto& dnc = context_.scene_.GetDayNightCycle();
        for (int i = 0; i < steps; ++i)
        {
            dnc.Update(context_.time_.deltaTime);
            componentController_.CallFunctions(FunctionType::Update);
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
        LOG_DEBUG << "Target time: " << time;
        auto diff = CalculateStepsToTime(time.hour, time.minute);
        if (diff > 0)
        {
            const auto lastDeltaTime = context_.time_.deltaTime;
            context_.time_.deltaTime = diff * 1000;
            SimulateTimeSteps(1);
            context_.time_.deltaTime = lastDeltaTime;
        }
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
        const auto hms     = context_.scene_.GetDayNightCycle().GetHourMinuteSecond();
        const auto current = hms.hour * 60 + hms.minute;
        const auto target  = ((targetHour * 60 + targetMinute) % MINUTES_PER_DAY + MINUTES_PER_DAY) % MINUTES_PER_DAY;
        const auto diff    = (target - current + MINUTES_PER_DAY) % MINUTES_PER_DAY;
        return diff;
    }

    void PrintCurrentTime()
    {
        LOG_DEBUG << "Current time" << context_.scene_.GetDayNightCycle();
    }

    void SetHour(const GameTime::HourMinuteSecond& hour)
    {
        auto& dnc = context_.scene_.GetDayNightCycle();
        dnc.SetHourMinuteSecond(hour);
        PrintCurrentTime();
    }
    void SetUp() override
    {
        PrintCurrentTime();
        LOG_DEBUG << "Test case started.";
    }

    AIController sut_;
    CharacterStatsComponent* stats_{nullptr};
    RoutineComponent* routine_{nullptr};
    ScoutProfessionComponent* profession_{nullptr};
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

    AdvanceToTime(GameTime::HourMinuteSecond{.hour = 14, .minute = 0});
    ASSERT_EQ(routine_->GetCurrentBehaviorState(), AIBehaviorState::Rest);

    AdvanceToTime(GameTime::HourMinuteSecond{.hour = 22, .minute = 0});
    ASSERT_EQ(routine_->GetCurrentBehaviorState(), AIBehaviorState::Sleep);
}

TEST_F(AIControllerTests, Jump_DailyCycleContinuity)
{
    routine_->AddStep(RoutineStep{.startHour = 12, .startMinute = 0, .behaviorState = AIBehaviorState::Eat});
    routine_->AddStep(RoutineStep{.startHour = 14, .startMinute = 0, .behaviorState = AIBehaviorState::Rest});

    SetHour(GameTime::HourMinuteSecond{.hour = 21, .minute = 59, .second = 59});
    ASSERT_EQ(routine_->GetCurrentBehaviorState(), AIBehaviorState::Work);

    AdvanceToTime(GameTime::HourMinuteSecond{.hour = 22, .minute = 0});
    ASSERT_EQ(routine_->GetCurrentBehaviorState(), AIBehaviorState::Sleep);

    AdvanceToTime(GameTime::HourMinuteSecond{.hour = 14, .minute = 0});
    ASSERT_EQ(routine_->GetCurrentBehaviorState(), AIBehaviorState::Rest);
}