#pragma once
#include <string>
#include <vector>

#include "AIBehaviorState.h"
#include "GameEngine/Components/Component.h"
#include "GameEngine/Components/VectorOfCustomStructure.h"

namespace GameEngine
{
namespace Components
{
struct RoutineStep
{
    int startHour   = 0;
    int startMinute = 0;
    AIBehaviorState behaviorState;
    vec3 targetPosition;

    int GetMinutesFromStartOfDay() const;
};

DECLARE_COMPONENT(RoutineComponent)
{
public:
    // clang-format off
    BEGIN_FIELDS()
        FIELD_VECTOR_OF_CUSTOM(routineSteps_)
        FIELD_FLOAT(debugCurrentGameTime_)
        FIELD_STRING(debugCurrentState_)
        FIELD_VECTOR3F(debugTargetPosition_)
    END_FIELDS()
    // clang-format on

public:
    RoutineComponent(ComponentContext&, GameObject&);
    ~RoutineComponent() override;
    void CleanUp() override;
    void ReqisterFunctions() override;
    void Reload() override;

public:
    void read(const TreeNode&) override;
    void write(TreeNode&) const override;

public:
    const std::vector<RoutineStep>& GetSchedule() const
    {
        return schedule_;
    }
    void AddStep(const RoutineStep& step);

    RoutineStep UpdateAndGetCurrentStep(const vec2i& currentHourAndMinutes);

private:
    void InitializeEditorStructures();

private:
    std::vector<RoutineStep> schedule_;
    VectorOfCustomStructure routineSteps_;

    float debugCurrentGameTime_    = 0.0f;
    AIBehaviorState debugCurrentState_ = AIBehaviorState::Idle;
    vec3 debugTargetPosition_;
};
}  // namespace Components
}  // namespace GameEngine