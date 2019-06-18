#pragma once
#include <Common/Controllers/CharacterController/Character.h>
#include "GameEngine/Components/BaseComponent.h"
#include <GameEngine/Components/Physics/Rigidbody.h>

namespace GameEngine
{
namespace Components
{
class CharacterController : public BaseComponent
{
public:
    enum class Action
    {
        IDLE = 0,
        MOVE_FORWARD,
        MOVE_BACKWARD,
        ROTATE_LEFT,
        ROTATE_RIGHT,
        RUN,
        WALK,
        JUMP
    };

    CharacterController(const ComponentContext& componentContext, GameObject& gameObject);

    virtual void ReqisterFunctions() override;

    void Init();
    void Update();

    void AddState(Action action);
    void RemoveState(Action action);

    void SetRunSpeed(float);
    void SetTurnSpeed(float);
    void SetJumpPower(float);

private:
    std::vector<Action> actions_;
    Rigidbody* rigidbody_;
    float jumpPower_;
    float turnSpeed_;
    float runSpeed_;

public:
    static ComponentsType type;
};
}  // namespace Components
}  // namespace GameEngine
