#pragma once
#include <memory>
#include <string>
#include <vector>

#include "GameEngine/Components/Component.h"
#include "GameEngine/Components/VectorOfCustomStructure.h"
#include "RoutineStateMachine/RoutineContext.h"
#include "RoutineStateMachine/RoutineStateMachine.h"
#include "RoutineStep.h"

namespace GameEngine
{
namespace Components
{

DECLARE_COMPONENT(RoutineComponent)
{
public:
    // clang-format off
    BEGIN_FIELDS()
        FIELD_VECTOR_OF_CUSTOM(routineSteps_)
        FIELD_FLOAT(debugCurrentGameTime_)
        FIELD_VECTOR3F(debugTargetPosition_)
    END_FIELDS()
    // clang-format on

public:
    RoutineComponent(ComponentContext&, GameObject&);
    ~RoutineComponent() override;

    void CleanUp() override;
    void ReqisterFunctions() override;
    void Reload() override;

    void Update();
    void RegisterProfession(ProfessionComponent&);

public:
    void read(const TreeNode&) override;
    void write(TreeNode&) const override;

public:
    const std::vector<RoutineStep>& GetSchedule() const
    {
        return schedule_;
    }
    void AddStep(const RoutineStep& step);

    std::optional<RoutineStep> UpdateAndGetCurrentStep(const vec2i&);

    void InitializeStateMachine();
    void HandleEvent(const RoutineAIEvent&);
    void UpdateStateMachine(float);

private:
    void InitializeEditorStructures();
    RoutineAIEvent CreateEvent(const RoutineStep&);

private:
    std::vector<RoutineStep> schedule_;
    VectorOfCustomStructure routineSteps_;

    float debugCurrentGameTime_ = 0.0f;
    vec3 debugTargetPosition_;

    std::unique_ptr<RoutineStateMachine> stateMachine_;
    AIBehaviorState currentBehaviorState_{AIBehaviorState::None};
    RoutineContext context;
};
}  // namespace Components
}  // namespace GameEngine