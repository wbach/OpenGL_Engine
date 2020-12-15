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
        MOVE_LEFT,
        MOVE_RIGHT,
        ROTATE_LEFT,
        ROTATE_RIGHT,
        RUN,
        WALK,
        JUMP
    };

    CharacterController(ComponentContext& componentContext, GameObject& gameObject);

    void CleanUp() override;
    void ReqisterFunctions() override;

    void Init();
    void Update();

    void AddState(Action);
    void RemoveState(Action);

    void SetRunSpeed(float);
    void SetTurnSpeed(float);
    void SetJumpPower(float);
    void SetJumpCallback(std::function<void()>);

private:
    std::vector<Action> actions_;
    std::function<void()> jumpCallback_;
    Rigidbody* rigidbody_;
    float jumpPower_;
    float turnSpeed_;
    float runSpeed_;
    bool isJumping_;

public:
    static ComponentsType type;
};
}  // namespace Components
}  // namespace GameEngine
