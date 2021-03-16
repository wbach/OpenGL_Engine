#include "EnemyController.h"

#include <Utils/math.hpp>

#include "ControllerUtlis.h"
#include "GameEngine/Animations/AnimationClip.h"
#include "GameEngine/Components/Characters/Enemy.h"
#include "GameEngine/Components/Characters/Player.h"
#include "GameEngine/Components/ComponentContext.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Components/Physics/CapsuleShape.h"
#include "GameEngine/Objects/GameObject.h"

namespace GameEngine
{
namespace Components
{
namespace
{
const float playerDetectionRange{10.f};
const auto moveSpeed   = Utils::KmToMs(8.f);
const auto rotateSpeed = 1.f;

const std::string COMPONENT_STR = "EnemyController";
}  // namespace

EnemyController::EnemyController(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(typeid(EnemyController).hash_code(), componentContext, gameObject)
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

    calculateMovingPoints();
}
void EnemyController::Update()
{
    if (not characterController_ or not enemy_ or not characterController_->fsm())
        return;

    auto& fsm = *characterController_->fsm();

    auto [distance, vectorToPlayer, componentPtr] = getComponentsInRange<Player>(
        componentContext_.componentController_, thisObject_.GetWorldTransform().GetPosition());

    if (componentPtr and distance < playerDetectionRange)
    {
        if (distance < (enemy_->characterStatistic().attackRange + characterController_->getShapeSize()))
        {
            fsm.handle(EndMoveEvent{});
            fsm.handle(AttackEvent{});
        }
        else
        {
            fsm.handle(EndAttackEvent{});
            fsm.handle(MoveForwardEvent{moveSpeed});
        }

        fsm.handle(RotateTargetEvent{rotateSpeed, caclulateTargetRotation(vectorToPlayer)});
        return;
    }

    auto vectorToTarget = freeWalkingTargetPoint - thisObject_.GetWorldTransform().GetPosition();
    fsm.handle(EndAttackEvent{});
    fsm.handle(RotateTargetEvent{rotateSpeed, caclulateTargetRotation(vectorToTarget)});
    fsm.handle(MoveForwardEvent{moveSpeed});

    auto distanceToPoint = glm::length(vectorToTarget);
    if (distanceToPoint < 5.f)
    {
        freeWalkingTargetPoint = movingPoints_[freeWalkingTargetPointIndex];
        ++freeWalkingTargetPointIndex;

        if (freeWalkingTargetPointIndex >= movingPoints_.size())
        {
            freeWalkingTargetPointIndex = 0;
        }
    }
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
void EnemyController::calculateMovingPoints()
{
    auto position = thisObject_.GetWorldTransform().GetPosition();

    const float range  = 10.f;
    const float offset = 5;

    movingPoints_[0] = position + vec3(getRandomFloat() * range + offset, 0, getRandomFloat() * range + offset);
    movingPoints_[1] = position + vec3(getRandomFloat() * range + offset, 0, -getRandomFloat() * range - offset);
    movingPoints_[2] = position + vec3(-getRandomFloat() * range - offset, 0, -getRandomFloat() * range - offset);
    movingPoints_[3] = position + vec3(-getRandomFloat() * range - offset, 0, getRandomFloat() * range + offset);

    freeWalkingTargetPointIndex = 0;
    freeWalkingTargetPoint      = movingPoints_[freeWalkingTargetPointIndex];
}
void EnemyController::registerReadFunctions()
{
    auto readFunc = [](ComponentContext& componentContext, const TreeNode&, GameObject& gameObject) {
        return std::make_unique<EnemyController>(componentContext, gameObject);
    };
    regsiterComponentReadFunction(COMPONENT_STR, readFunc);
}
void EnemyController::write(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, COMPONENT_STR});
}
}  // namespace Components
}  // namespace GameEngine
