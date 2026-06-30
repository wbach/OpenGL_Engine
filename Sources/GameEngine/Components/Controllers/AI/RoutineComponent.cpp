#include "RoutineComponent.h"

#include <Utils/TreeNodeReadFunctions.h>
#include <Utils/TreeNodeWriteFunctions.h>

#include <algorithm>

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
{
    InitializeEditorStructures();
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

void RoutineComponent::CleanUp()
{
    schedule_.clear();
    routineSteps_.clear();
}

void RoutineComponent::Reload()
{
    routineSteps_.init();
}

void RoutineComponent::ReqisterFunctions()
{
}

void RoutineComponent::AddStep(const RoutineStep& step)
{
    schedule_.push_back(step);
    routineSteps_.init();
}

int RoutineStep::GetMinutesFromStartOfDay() const
{
    return (startHour * 60) + startMinute;
}

RoutineStep RoutineComponent::UpdateAndGetCurrentStep(const vec2i& currentHourAndMinutes)
{
    int currentGlobalMinutes = (currentHourAndMinutes.x * 60) + currentHourAndMinutes.y;

    if (schedule_.empty())
    {
        return RoutineStep{};
    }

    std::sort(schedule_.begin(), schedule_.end(),
              [](const RoutineStep& a, const RoutineStep& b)
              { return a.GetMinutesFromStartOfDay() < b.GetMinutesFromStartOfDay(); });

    RoutineStep foundStep = schedule_.back();

    for (auto it = schedule_.rbegin(); it != schedule_.rend(); ++it)
    {
        if (currentGlobalMinutes >= it->GetMinutesFromStartOfDay())
        {
            foundStep = *it;
            break;
        }
    }

    debugCurrentState_   = foundStep.behaviorState;
    debugTargetPosition_ = foundStep.targetPosition;

    return foundStep;
}

void RoutineComponent::read(const TreeNode& node)
{
    schedule_.clear();

    if (auto snode = node.getChild(CSTR_SCHEDULE))
    {
        Components::Read(*snode, schedule_);
    }

    routineSteps_.init();
}

void RoutineComponent::write(TreeNode& node) const
{
    if (!schedule_.empty())
    {
        Components::write(node.addChild(CSTR_SCHEDULE), schedule_);
    }
}

}  // namespace Components
}  // namespace GameEngine