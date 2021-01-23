#include "EnemyController.h"

#include <Utils/math.hpp>

#include "ControllerUtlis.h"
#include "GameEngine/Animations/AnimationClip.h"
#include "GameEngine/Components/Characters/Enemy.h"
#include "GameEngine/Components/Characters/Player.h"
#include "GameEngine/Components/ComponentContext.h"
#include "GameEngine/Objects/GameObject.h"

namespace GameEngine
{
namespace Components
{
namespace
{
const float playerDetectionRange{10.f};
}  // namespace

ComponentsType EnemyController::type = ComponentsType::EnemyController;

EnemyController::EnemyController(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(type, componentContext, gameObject)
    , enemy_{nullptr}
    , animator_{nullptr}
    , characterController_{nullptr}
{
}
void EnemyController::CleanUp()
{
}
void EnemyController::ReqisterFunctions()
{
    RegisterFunction(FunctionType::OnStart, [this]() { Init(); });
    RegisterFunction(FunctionType::Update, [this]() { Update(); });
}
void EnemyController::Init()
{
    animator_            = thisObject_.GetComponent<Animator>();
    characterController_ = thisObject_.GetComponent<CharacterController>();
    enemy_               = thisObject_.GetComponent<Enemy>();
}
void EnemyController::Update()
{
    if (not characterController_ and not enemy_)
        return;

    auto [distance, vectorToPlayer, componentPtr] = getComponentsInRange<Player>(
        componentContext_.componentController_, thisObject_.GetWorldTransform().GetPosition());

    if (componentPtr and distance < playerDetectionRange)
    {
        if (distance > enemy_->characterStatistic().attackRange)
        {
            characterController_->addState(std::make_unique<MoveForward>(Utils::KmToMs(8.f)));
        }
        else
        {
            characterController_->removeState(CharacterControllerState::Type::MOVE_FORWARD);
            characterController_->addState(std::make_unique<Attack>());
        }
        characterController_->addState(std::make_unique<RotateToTarget>(caclulateTargetRotation(vectorToPlayer)));
        return;
    }

    clearStates();
}
Quaternion EnemyController::caclulateTargetRotation(const vec3& toPlayer) const
{
    auto angle2 = atan2f(toPlayer.x, toPlayer.z);
    Quaternion targertRotation;
    targertRotation.x = 0.f;
    targertRotation.y = 1.f * sinf(angle2 / 2.f);
    targertRotation.z = 0.f;
    targertRotation.w = cosf(angle2 / 2.f);

    return targertRotation;
}
void EnemyController::clearStates()
{
    characterController_->removeState(CharacterControllerState::Type::ATTACK);
    characterController_->removeState(CharacterControllerState::Type::ROTATE_TARGET);
    characterController_->removeState(CharacterControllerState::Type::MOVE_FORWARD);
}
}  // namespace Components
}  // namespace GameEngine
