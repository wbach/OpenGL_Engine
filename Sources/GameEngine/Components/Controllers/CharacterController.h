#pragma once
#include <Common/Controllers/CharacterController/Character.h>
#include <GameEngine/Components/Physics/Rigidbody.h>

#include <Utils/math.hpp>
#include <unordered_map>

#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/BaseComponent.h"

namespace GameEngine
{
namespace Components
{
const float DEFAULT_RUN_SPEED  = Utils::KmToMs(12.f);
const float DEFAULT_TURN_SPEED = 160.f;
const float DEFAULT_JUMP_POWER = 10.f;

class CharacterControllerState
{
public:
    enum class Type
    {
        IDLE = 0,
        MOVE_FORWARD,
        MOVE_BACKWARD,
        MOVE_LEFT,
        MOVE_RIGHT,
        ROTATE_LEFT,
        ROTATE_RIGHT,
        ROTATE_TARGET,
        JUMP,
        HURT,
        ATTACK,
        DEATH
    };

    struct Context
    {
        Components::Rigidbody& rigidBody;
    };

    CharacterControllerState(Type type)
        : type_{type}
        , rigidbody_{nullptr}
    {
    }

    virtual ~CharacterControllerState() = default;

    void setup(Components::Rigidbody* rigidbody)
    {
        rigidbody_ = rigidbody;
    }

    virtual bool onStart()
    {
        return true;
    }
    virtual bool isUpToDate(const CharacterControllerState&) const
    {
        return true;
    }
    virtual void process(float)
    {
    }
    virtual bool isEnd() const
    {
        return false;
    }

    Type getType() const
    {
        return type_;
    }

protected:
    Components::Rigidbody* rigidbody_;

private:
    Type type_;
};

class Move : public CharacterControllerState
{
public:
    Move(CharacterControllerState::Type type, float moveSpeed, const vec3& directon)
        : CharacterControllerState(type)
        , moveSpeed_{moveSpeed}
        , directon_{glm::normalize(directon)}
    {
    }

    void process(float) override
    {
        auto targetVelocity = rigidbody_->GetRotation() * directon_;
        targetVelocity      = glm::normalize(targetVelocity);
        targetVelocity *= moveSpeed_;

        auto velocity       = rigidbody_->GetVelocity();
        auto velocityChange = (targetVelocity - velocity);
        velocityChange.x    = glm::clamp(velocityChange.x, -moveSpeed_, moveSpeed_);
        velocityChange.z    = glm::clamp(velocityChange.z, -moveSpeed_, moveSpeed_);
        velocityChange.y    = 0;

        auto newVelocity = velocity + velocityChange;
        rigidbody_->SetVelocity(newVelocity);
    }

private:
    const float moveSpeed_;
    const vec3 directon_;
};

class MoveForward : public Move
{
public:
    MoveForward(float moveSpeed = DEFAULT_RUN_SPEED)
        : Move(CharacterControllerState::Type::MOVE_FORWARD, moveSpeed, vec3(0.f, 0.f, 1.f))
    {
    }
};

class MoveBackward : public Move
{
public:
    MoveBackward(float moveSpeed = DEFAULT_RUN_SPEED)
        : Move(CharacterControllerState::Type::MOVE_BACKWARD, moveSpeed, vec3(0.f, 0.f, -1.f))
    {
    }
};

class Rotate : public CharacterControllerState
{
public:
    Rotate(CharacterControllerState::Type type, float rotateSpeed)
        : CharacterControllerState(type)
        , rotateSpeed_{rotateSpeed}
    {
    }

    void process(float deltaTime) override
    {
        auto rotation = rigidbody_->GetRotation() *
                        glm::angleAxis(glm::radians(rotateSpeed_ * deltaTime), glm::vec3(0.f, 1.f, 0.f));
        rigidbody_->SetRotation(rotation);
    }

private:
    float rotateSpeed_;
};

class RotateLeft : public Rotate
{
public:
    RotateLeft(float speed = DEFAULT_TURN_SPEED)
        : Rotate(CharacterControllerState::Type::ROTATE_LEFT, speed)
    {
    }
};

class RotateRight : public Rotate
{
public:
    RotateRight(float speed = DEFAULT_TURN_SPEED)
        : Rotate(CharacterControllerState::Type::ROTATE_RIGHT, -speed)
    {
    }
};

class RotateToTarget : public CharacterControllerState
{
public:
    RotateToTarget(const Quaternion& target, float speed = DEFAULT_TURN_SPEED)
        : CharacterControllerState(CharacterControllerState::Type::ROTATE_TARGET)
        , rotateSpeed_{speed}
        , progress_{0.f}
        , rotateTarget_{target}
        , toRemove_{false}
    {
    }

    bool isUpToDate(const CharacterControllerState& incomingState) const
    {
        if (incomingState.getType() == getType())
        {
            auto incomingRotateState = static_cast<const RotateToTarget*>(&incomingState);
            return incomingRotateState->rotateTarget_ == rotateTarget_;
        }

        return false;
    }

    bool onStart() override
    {
        startedRotation_ = rigidbody_->GetRotation();
        return startedRotation_ != rotateTarget_;
    }

    void process(float deltaTime) override
    {
        progress_ += deltaTime;

        if (progress_ > 1.f)
        {
            progress_ = 1.f;
            toRemove_ = true;
        }

        auto newRotation = glm::slerp(startedRotation_, rotateTarget_, progress_);
        rigidbody_->SetRotation(newRotation);
    }

    bool isEnd() const override
    {
        return toRemove_;
    }

private:
    float rotateSpeed_;
    float progress_;
    Quaternion startedRotation_;
    Quaternion rotateTarget_;
    bool toRemove_;
};

class Jump : public CharacterControllerState
{
public:
    Jump(float jumpPower = DEFAULT_JUMP_POWER)
        : CharacterControllerState(CharacterControllerState::Type::JUMP)
        , power_{jumpPower}
    {
    }

    bool onStart() override
    {
        auto velocity = rigidbody_->GetVelocity();
        velocity.y += power_;
        rigidbody_->SetVelocity(velocity);
        return true;
    }

private:
    float power_;
};

class Hurt : public CharacterControllerState
{
public:
    Hurt()
        : CharacterControllerState(CharacterControllerState::Type::HURT)
    {
    }
};

class Attack : public CharacterControllerState
{
public:
    Attack()
        : CharacterControllerState(CharacterControllerState::Type::ATTACK)
    {
    }
};

class Death : public CharacterControllerState
{
public:
    Death()
        : CharacterControllerState(CharacterControllerState::Type::DEATH)
    {
    }
};

class CharacterController : public BaseComponent
{
    using States = std::vector<std::unique_ptr<CharacterControllerState>>;

public:
    CharacterController(ComponentContext& componentContext, GameObject& gameObject);

    void CleanUp() override;
    void ReqisterFunctions() override;

    void Init();
    void Update();

    void addState(std::unique_ptr<CharacterControllerState>);
    void removeState(CharacterControllerState::Type);
    void clearStates();

    void SetRunSpeed(float);
    void SetTurnSpeed(float);
    void SetJumpPower(float);

public:
    std::string hurtAnimationName;
    std::string attackAnimationName;
    std::string deathAnimationName;
    std::string jumpAnimationName;
    std::string moveForwardAnimationName;
    std::string moveBackwardAnimationName;
    std::string idleAnimationName;

private:
    void isOnGround();
    void onStateAdittion(CharacterControllerState::Type);
    void onStateRemove(CharacterControllerState::Type);
    void processStates();
    void clearVelocityIfNotMoving();

    void setAnimation(const std::string&, PlayDirection = PlayDirection::forward);

    bool isState(CharacterControllerState::Type);
    bool isStates(const std::vector<CharacterControllerState::Type>&);
    States::iterator findState(CharacterControllerState::Type);

private:
    std::vector<std::unique_ptr<CharacterControllerState>> states_;
    std::unordered_map<CharacterControllerState::Type, std::string*> stateTypeToAnimName_;
    std::function<void()> jumpCallback_;
    Rigidbody* rigidbody_;
    Animator* animator_;

    float jumpPower_;
    float turnSpeed_;
    float runSpeed_;

private:
    Quaternion rotateTarget;
    Quaternion startedRotation;
    float progress_;
    std::mutex stateMutex_;

public:
    static ComponentsType type;
};
}  // namespace Components
}  // namespace GameEngine
