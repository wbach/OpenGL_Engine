#include "EnemyController.h"

#include <Utils/math.hpp>

#include "GameEngine/Animations/AnimationClip.h"
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
const float ATTACK_RANGE{2.f};
}  // namespace

ComponentsType EnemyController::type = ComponentsType::EnemyController;

EnemyController::EnemyController(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(type, componentContext, gameObject)
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

    if (animator_)
    {
        animator_->onAnimationEnd_[characterController_->attackAnimationName].push_back([this]() {
            if (closestPlayerComponent_)
            {
                closestPlayerComponent_->hurt();
            }
        });
    }
}
void EnemyController::Update()
{
    if (not characterController_)
        return;

    auto& playerComponents = componentContext_.componentController_.GetAllComonentsOfType(ComponentsType::Player);
    const auto& thisEnemyPosition = thisObject_.GetWorldTransform().GetPosition();

    closestPlayerComponent_ = nullptr;
    vec3 toPlayer{0.f};

    float distance = std::numeric_limits<float>::max();

    for (const auto& playerComponent : playerComponents)
    {
        const auto& playerPosition = playerComponent.second->getParentGameObject().GetWorldTransform().GetPosition();

        const auto dist              = playerPosition - thisEnemyPosition;
        const auto& distanceToPlayer = glm::length(dist);

        if (distanceToPlayer < distance)
        {
            distance                = distanceToPlayer;
            toPlayer                = dist;
            closestPlayerComponent_ = static_cast<Player*>(playerComponent.second);
        }
    }

    if (closestPlayerComponent_ and distance < playerDetectionRange)
    {
        if (distance > ATTACK_RANGE)
        {
            characterController_->addState(std::make_unique<MoveForward>(Utils::KmToMs(8.f)));
        }
        else
        {
            characterController_->removeState(CharacterControllerState::Type::MOVE_FORWARD);
            characterController_->addState(std::make_unique<Attack>());
        }
        characterController_->addState(std::make_unique<RotateToTarget>(caclulateTargetRotation(toPlayer)));
        return;
    }

    clearStates();
}
Quaternion EnemyController::caclulateTargetRotation(const vec3& toPlayer) const
{
    auto angle2 = atan2(toPlayer.x, toPlayer.z);
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
