#include "CharacterController.h"
#include <Utils/math.hpp>
#include "GameEngine/Objects/GameObject.h"

namespace GameEngine
{
namespace Components
{
ComponentsType CharacterController::type = ComponentsType::CharacterController;

const float DEFAULT_RUN_SPEED = Utils::KmToMs(5.f);
// const float DEFAULT_RUN_TURN_SPEED = 160.f;
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

    for (const auto& action : actions_)
    {
        switch (action)
        {
            case Action::RUN:
            case Action::WALK:
                rigidbody_->SetVelocity(vec3(DEFAULT_RUN_SPEED, 0, DEFAULT_RUN_SPEED));
                break;
            case Action::JUMP:
                rigidbody_->SetVelocity(vec3(0, DEFAULT_JUMP_POWER, 0));
                break;

            default:
                break;
        }
    }
}

void CharacterController::AddState(CharacterController::Action action)
{
    actions_.push_back(action);
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
