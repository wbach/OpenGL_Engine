#include "CharacterController.h"
#include <Utils/math.hpp>
#include "GameEngine/Objects/GameObject.h"

namespace GameEngine
{
namespace Components
{
ComponentsType CharacterController::type = ComponentsType::CharacterController;

const float DEFAULT_RUN_SPEED      = Utils::KmToMs(5.f);
const float DEFAULT_RUN_TURN_SPEED = 160.f;
const float DEFAULT_JUMP_POWER     = 25.f;

CharacterController::CharacterController(const ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(type, componentContext, gameObject)
    , characterController_(gameObject.worldTransform, DEFAULT_RUN_SPEED, DEFAULT_RUN_TURN_SPEED, DEFAULT_JUMP_POWER)
{
}

void CharacterController::ReqisterFunctions()
{
    RegisterFunction(FunctionType::Update, std::bind(&CharacterController::Update, this));
}

void CharacterController::Update()
{
    characterController_.Update(componentContext_.time_.deltaTime);
}

void CharacterController::SetJumpPower(float v)
{
    characterController_.SetJumpPower(v);
}

common::Controllers::CharacterController & CharacterController::Get()
{
    return characterController_;
}

void CharacterController::SetTurnSpeed(float v)
{
    characterController_.SetTurnSpeed(v);
}

void CharacterController::SetRunSpeed(float v)
{
    characterController_.SetRunSpeed(v);
}
}  // namespace Components
}  // namespace GameEngine
