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
    , idleAnimationName_{"Idle"}
    , runAnimationName_{"Walk"}
    , attackAnimationName_{"Pound"}
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

    if (characterController_)
    {
        characterController_->SetRunSpeed(Utils::KmToMs(8.f));
    }
}
void EnemyController::Update()
{
    if (not characterController_)
        return;

    auto& playerComponents = componentContext_.componentController_.GetAllComonentsOfType(ComponentsType::Player);
    const auto& thisEnemyPosition = thisObject_.GetWorldTransform().GetPosition();

    Player* closestPlayerComponent{nullptr};
    vec3 toPlayer{0.f};

    float distance = std::numeric_limits<float>::max();

    for (const auto& playerComponent : playerComponents)
    {
        const auto& playerPosition = playerComponent.second->getParentGameObject().GetWorldTransform().GetPosition();

        const auto dist              = playerPosition - thisEnemyPosition;
        const auto& distanceToPlayer = glm::length(dist);

        if (distanceToPlayer < distance)
        {
            distance               = distanceToPlayer;
            toPlayer               = dist;
            closestPlayerComponent = static_cast<Player*>(playerComponent.second);
        }
    }

    if (closestPlayerComponent)
    {
        if (distance < playerDetectionRange)
        {
            if (distance > ATTACK_RANGE)
            {
                if (animator_)
                {
                    if (animator_->GetCurrentAnimationName() != runAnimationName_)
                        animator_->ChangeAnimation(runAnimationName_, Animator::AnimationChangeType::direct);
                }

                characterController_->AddState(CharacterController::Action::MOVE_FORWARD);
            }
            else
            {
                if (animator_)
                {
                    if (animator_->GetCurrentAnimationName() != attackAnimationName_)
                        animator_->ChangeAnimation(attackAnimationName_, Animator::AnimationChangeType::direct);
                }
                characterController_->RemoveState(CharacterController::Action::MOVE_FORWARD);
            }

            auto angle2 = atan2(toPlayer.x, toPlayer.z);
            Quaternion targertRotation;
            targertRotation.x = 0.f;
            targertRotation.y = 1.f * sinf(angle2 / 2.f);
            targertRotation.z = 0.f;
            targertRotation.w = cosf(angle2 / 2.f);
            characterController_->setRotationTarget(targertRotation);
        }
        else
        {
            if (animator_)
            {
                if (animator_->GetCurrentAnimationName() != idleAnimationName_)
                    animator_->ChangeAnimation(idleAnimationName_, Animator::AnimationChangeType::direct);
            }
            characterController_->RemoveState(CharacterController::Action::MOVE_FORWARD);
        }
    }
    else
    {
        if (animator_)
        {
            if (animator_->GetCurrentAnimationName() != idleAnimationName_)
                animator_->ChangeAnimation(idleAnimationName_, Animator::AnimationChangeType::direct);
        }
        characterController_->RemoveState(CharacterController::Action::MOVE_FORWARD);
    }
}
}  // namespace Components
}  // namespace GameEngine
