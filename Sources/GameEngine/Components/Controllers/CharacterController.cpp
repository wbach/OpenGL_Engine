#include "CharacterController.h"

#include <Utils/math.hpp>

#include "GameEngine/Objects/GameObject.h"

namespace GameEngine
{
namespace Components
{
ComponentsType CharacterController::type = ComponentsType::CharacterController;

const float DEFAULT_RUN_SPEED  = Utils::KmToMs(12.f);
const float DEFAULT_TURN_SPEED = 160.f;
const float DEFAULT_JUMP_POWER = 25.f;

CharacterController::CharacterController(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(type, componentContext, gameObject)
    , rigidbody_{nullptr}
    , jumpPower_(DEFAULT_JUMP_POWER)
    , turnSpeed_(DEFAULT_TURN_SPEED)
    , runSpeed_(DEFAULT_RUN_SPEED)
{
}

void CharacterController::CleanUp()
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

    if (rigidbody_)
        rigidbody_->InputParams().angularFactor_ = vec3(0);
}

void CharacterController::Update()
{
    if (not rigidbody_ or not rigidbody_->IsReady())
    {
        return;
    }

    vec3 direction(0.f);
    for (const auto& action : actions_)
    {
        switch (action)
        {
            case Action::MOVE_BACKWARD:
                direction.z = -1.f;
                break;
            case Action::MOVE_FORWARD:
                direction.z = 1.f;
                break;
            case Action::JUMP:
                direction.y += jumpPower_;
                break;
            case Action::MOVE_LEFT:
                direction.x = 1.f;
                break;
            case Action::MOVE_RIGHT:
                direction.x = -1.f;
                break;
            case Action::ROTATE_LEFT:
            {
                auto rotation = rigidbody_->GetRotation() *
                                glm::angleAxis(glm::radians(turnSpeed_ * componentContext_.time_.deltaTime),
                                               glm::vec3(0.f, 1.f, 0.f));
                rigidbody_->SetRotation(rotation);
            }
            break;
            case Action::ROTATE_RIGHT:
            {
                auto rotation = rigidbody_->GetRotation() *
                                glm::angleAxis(glm::radians(-turnSpeed_ * componentContext_.time_.deltaTime),
                                               glm::vec3(0.f, 1.f, 0.f));
                rigidbody_->SetRotation(rotation);
            }
            break;
            default:
                DEBUG_LOG("unknown action!");
                break;
        }
    }

    if (glm::length(direction) < std::numeric_limits<float>::epsilon())
    {
        rigidbody_->SetVelocity(vec3(0));
        return;
    }

    auto targetVelocity = rigidbody_->GetRotation() * direction;
    targetVelocity      = glm::normalize(targetVelocity);
    targetVelocity *= runSpeed_;

    auto velocity       = rigidbody_->GetVelocity();
    auto velocityChange = (targetVelocity - velocity);
    velocityChange.x    = glm::clamp(velocityChange.x, -runSpeed_, runSpeed_);
    velocityChange.z    = glm::clamp(velocityChange.z, -runSpeed_, runSpeed_);
    velocityChange.y    = 0;

    auto newVelocity = velocity + velocityChange;
    rigidbody_->SetVelocity(newVelocity);
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
