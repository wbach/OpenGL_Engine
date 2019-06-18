#include "CharacterController.h"
#include <Utils/math.hpp>
#include "GameEngine/Objects/GameObject.h"

namespace GameEngine
{
namespace Components
{
ComponentsType CharacterController::type = ComponentsType::CharacterController;

const float DEFAULT_RUN_SPEED = Utils::KmToMs(15.f);
const float DEFAULT_TURN_SPEED = 160.f;
const float DEFAULT_JUMP_POWER = 25.f;

CharacterController::CharacterController(const ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(type, componentContext, gameObject)
    , rigidbody_{nullptr}
{
}

void CharacterController::ReqisterFunctions()
{
    RegisterFunction(FunctionType::Awake, std::bind(&CharacterController::Init, this));
    RegisterFunction(FunctionType::Update, std::bind(&CharacterController::Update, this));
}

void CharacterController::Init()
{
    rigidbody_ = thisObject_.GetComponent<Rigidbody>();
}

void CharacterController::Update()
{
    if (not rigidbody_)
    {
        return;
    }
    const auto& currentVelocvity = rigidbody_->GetVelocity();
    float rad = Utils::ToRadians(rotation_.y);
    auto v    = vec3(DEFAULT_RUN_SPEED);
    v.x *= sinf(rad);
    v.y = currentVelocvity.y;
    v.z *= cosf(rad);

    for (const auto& action : actions_)
    {
        switch (action)
        {
            case Action::MOVE_BACKWARD:
                v.x *= -1.f;
                v.z *= -1.f;
                rigidbody_->SetVelocity(v);
                break;
            case Action::MOVE_FORWARD:
                rigidbody_->SetVelocity(v);
                break;
            case Action::JUMP:
                rigidbody_->SetVelocity(vec3(0, DEFAULT_JUMP_POWER, 0));
                break;
            case Action::ROTATE_LEFT:
            {
                rotation_.y += DEFAULT_TURN_SPEED * componentContext_.time_.deltaTime;
                auto v = Utils::ToRadians(rotation_);
                rigidbody_->SetRotation(v);
            }
            break;
            case Action::ROTATE_RIGHT:
            {
                rotation_.y -= DEFAULT_TURN_SPEED * componentContext_.time_.deltaTime;
                auto v = Utils::ToRadians(rotation_);
                rigidbody_->SetRotation(v);
            }
            break;

            default:
                break;
        }
    }
}

void CharacterController::AddState(CharacterController::Action action)
{
    auto iter = std::find(actions_.begin(), actions_.end(), action);

    if (iter == actions_.end())
    {
        actions_.push_back(action);
    }
}

void CharacterController::RemoveState(CharacterController::Action action)
{
    auto iter = std::find(actions_.begin(), actions_.end(), action);
    if (iter != actions_.end())
    {
        actions_.erase(iter);
    }
}

void CharacterController::SetJumpPower(float v)
{
    jumpPower_ = v;
}

void CharacterController::SetTurnSpeed(float v)
{
    turnSpeed_ = v;
}

void CharacterController::SetRunSpeed(float v)
{
    runSpeed_ = v;
}
}  // namespace Components
}  // namespace GameEngine
