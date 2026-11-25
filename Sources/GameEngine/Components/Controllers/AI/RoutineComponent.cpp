#include "RoutineComponent.h"

#include <Utils/TreeNodeReadFunctions.h>
#include <Utils/TreeNodeWriteFunctions.h>

#include <algorithm>
#include <optional>

#include "GameEngine/Components/ComponentContext.h"
#include "GameEngine/Scene/Scene.hpp"
#include "ProfessionComponent.h"

namespace GameEngine
{
namespace Components
{
namespace
{
const char* const CSTR_SCHEDULE     = "schedule";
const char* const CSTR_STEP         = "step";
const char* const CSTR_START_HOUR   = "start_hour";
const char* const CSTR_START_MINUTE = "start_minute";
const char* const CSTR_STATE        = "state";
const char* const CSTR_TARGET_POS   = "target_pos";
}  // namespace

void Read(const TreeNode& node, GameEngine::Components::RoutineStep& step)
{
    ::Read(node.getChild(CSTR_START_HOUR), step.startHour);
    ::Read(node.getChild(CSTR_START_MINUTE), step.startMinute);
    ::Read(node.getChild(CSTR_STATE), step.behaviorState);
    ::Read(node.getChild(CSTR_TARGET_POS), step.targetPosition);
}

void write(TreeNode& node, const GameEngine::Components::RoutineStep& step)
{
    ::write(node.addChild(CSTR_START_HOUR), step.startHour);
    ::write(node.addChild(CSTR_START_MINUTE), step.startMinute);
    ::write(node.addChild(CSTR_STATE), step.behaviorState);
    ::write(node.addChild(CSTR_TARGET_POS), step.targetPosition);
}

void Read(const TreeNode& node, std::vector<GameEngine::Components::RoutineStep>& steps)
{
    steps.clear();
    for (const auto& childNode : node.getChildren())
    {
        GameEngine::Components::RoutineStep step;
        Read(*childNode, step);
        steps.push_back(step);
    }
}

void write(TreeNode& node, const std::vector<GameEngine::Components::RoutineStep>& steps)
{
    for (const auto& step : steps)
    {
        auto& childNode = node.addChild(CSTR_STEP);
        write(childNode, step);
    }
}

REGISTER_COMPONENT(RoutineComponent)

RoutineComponent::RoutineComponent(ComponentContext& componentContext, GameObject& gameObject)
    : Component(componentContext, gameObject)
    , context(RoutineContext{.profession_ = nullptr, .gameObject = gameObject})
{
    InitializeEditorStructures();
    InitializeStateMachine();
}

RoutineComponent::~RoutineComponent()
{
}

void RoutineComponent::InitializeEditorStructures()
{
    auto createRoutineStepElement = [](std::vector<RoutineStep>& steps)
    {
        steps.emplace_back();
        auto& newStep = steps.back();

        return CustomStructure{.name   = "RoutineStep",
                               .fields = {{"Start Hour", FieldType::Int, &newStep.startHour},
                                          {"Start Minute", FieldType::Int, &newStep.startMinute},
                                          MakeEnumField("AI State", &newStep.behaviorState),
                                          {"Target Pos", FieldType::Vector3f, &newStep.targetPosition}}};
    };

    routineSteps_.createElement = [this, createRoutineStepElement]() { return createRoutineStepElement(schedule_); };

    auto initElements = [](std::vector<CustomStructure>& out, std::vector<RoutineStep>& steps)
    {
        for (auto& step : steps)
        {
            out.push_back(CustomStructure{.name   = "RoutineStep",
                                          .fields = {{"Start Hour", FieldType::Int, &step.startHour},
                                                     {"Start Minute", FieldType::Int, &step.startMinute},
                                                     MakeEnumField("AI State", &step.behaviorState),
                                                     {"Target Pos", FieldType::Vector3f, &step.targetPosition}}});
        }
    };
    routineSteps_.initElements = [this, initElements](std::vector<CustomStructure>& out) { initElements(out, schedule_); };
}
RoutineAIEvent RoutineComponent::CreateEvent(const RoutineStep& step)
{
    switch (step.behaviorState)
    {
        case AIBehaviorState::Work:
            return WorkStarted{.workLocation = step.targetPosition};

        case AIBehaviorState::Eat:
            return MealStarted{.foodSource = step.targetPosition};

        case AIBehaviorState::Sleep:
            return SleepStarted{};

        case AIBehaviorState::Rest:
            return RestStarted{};
    }

    throw std::runtime_error("Unknown routine behavior");
}

void RoutineComponent::InitializeStateMachine()
{
    stateMachine_ =
        std::make_unique<RoutineStateMachine>(WorkState{context}, RestState{}, EatState{}, SleepState{}, SuspendedState{});
}

void RoutineComponent::ReqisterFunctions()
{
    RegisterFunction(FunctionType::Update, [this]() { Update(); });
}

void RoutineComponent::CleanUp()
{
    schedule_.clear();
    routineSteps_.clear();
    stateMachine_.reset();
}

void RoutineComponent::Reload()
{
    routineSteps_.init();
}

void RoutineComponent::Update()
{
    if (!stateMachine_)
    {
        return;
    }

    const auto currentStep = UpdateAndGetCurrentStep(componentContext_.scene_.GetDayNightCycle());

    if (currentStep)
    {
        if (currentStep->behaviorState != currentBehaviorState_)
        {
            currentBehaviorState_ = currentStep->behaviorState;

            auto event = CreateEvent(*currentStep);
            std::visit(
                [&](auto e)
                {
                    stateMachine_->handle(e);
                },
                event);
        }
    }

    std::visit([&](auto statePtr) { statePtr->update(componentContext_.time_.deltaTime); }, stateMachine_->currentState);
}
void RoutineComponent::RegisterProfession(ProfessionComponent& component)
{
    LOG_DEBUG << "RegisterProfession: " << component.GetTypeName() << " " << thisObject_.GetName();
    context.profession_ = &component;
}

void RoutineComponent::AddStep(const RoutineStep& step)
{
    schedule_.push_back(step);
    SortRoutineSteps();
    routineSteps_.init();
}

int RoutineStep::GetMinutesFromStartOfDay() const
{
    return (startHour * 60) + startMinute;
}

std::optional<RoutineStep> RoutineComponent::UpdateAndGetCurrentStep(const GameTime& gameTime)
{
    auto currentGlobalMinutes = static_cast<int>(gameTime.GetMinutes());

    if (schedule_.empty())
    {
        LOG_DEBUG << "schedule_ is empty";
        return std::nullopt;
    }

    std::optional<RoutineStep> result;
    for (const auto& step : schedule_)
    {
        if (currentGlobalMinutes >= step.GetMinutesFromStartOfDay())
        {
            result = step;
        }
    }

    if (currentGlobalMinutes < schedule_.front().GetMinutesFromStartOfDay())
    {
        for (auto it = schedule_.rbegin(); it != schedule_.rend(); ++it)
        {
            if (it->GetMinutesFromStartOfDay() <= currentGlobalMinutes)
            {
                result = *it;
                break;
            }
        }
    }

    if (result)
        debugTargetPosition_ = result->targetPosition;

    return result;
}

void RoutineComponent::HandleEvent(const RoutineAIEvent& event)
{
    if (stateMachine_)
    {
        auto passEventToMachine = [&](const auto& e)
        {
            LOG_DEBUG << Utils::GetTypeName(e);
            stateMachine_->handle(e);
        };
        std::visit(passEventToMachine, event);
    }
    else
    {
        LOG_WARN << "No stateMachine";
    }
}

void RoutineComponent::UpdateStateMachine(float deltaTime)
{
}

void RoutineComponent::read(const TreeNode& node)
{
    schedule_.clear();

    if (auto snode = node.getChild(CSTR_SCHEDULE))
    {
        Components::Read(*snode, schedule_);
    }

    SortRoutineSteps();
    routineSteps_.init();
}

void RoutineComponent::write(TreeNode& node) const
{
    if (!schedule_.empty())
    {
        Components::write(node.addChild(CSTR_SCHEDULE), schedule_);
    }
}

void RoutineComponent::SortRoutineSteps()
{
    std::sort(schedule_.begin(), schedule_.end(),
              [](const RoutineStep& a, const RoutineStep& b)
              { return a.GetMinutesFromStartOfDay() < b.GetMinutesFromStartOfDay(); });
}
}  // namespace Components
}  // namespace GameEngine
