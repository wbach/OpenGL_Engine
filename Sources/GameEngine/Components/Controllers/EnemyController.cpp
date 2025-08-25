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
const auto moveSpeed = Utils::KmToMs(8.f);

const std::string COMPONENT_STR                    = "EnemyController";
const std::string CSTR_GENERATE_MOVING_PATH        = "generateMovingPath";
const std::string CSTR_GENERATE_MOVING_PATH_RANGE  = "range";
const std::string CSTR_GENERATE_MOVING_PATH_OFFSET = "ofsset";
const std::string CSTR_GENERATE_MOVING_PATH_ACTIVE = "active";
}  // namespace

EnemyController::EnemyController(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(COMPONENT_STR, componentContext, gameObject)
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

    if (generatePathParams.isActive)
        calculateMovingPoints();

    if (characterController_)
    {
        characterController_->moveSpeeds_.runSpeed.rotate = 3.f;
    }
}
void EnemyController::Update()
{
    if (not characterController_ or not enemy_)
        return;

    auto [distance, vectorToPlayer, componentPtr] = getComponentsInRange<Player>(
        componentContext_.componentController_, thisObject_.GetWorldTransform().GetPosition());

    if (componentPtr and distance < playerDetectionRange)
    {
        if (distance < (enemy_->characterStatistic().attackRange + characterController_->getShapeSize()))
        {
            DEBUG_LOG("In attack range");
            characterController_->pushEventToQueue(EndForwardMoveEvent{});
            //characterController_->pushEventToQueue(AttackEvent{});
        }
        else
        {
            //characterController_->pushEventToQueue(EndAttackEvent{});
            characterController_->pushEventToQueue(MoveForwardEvent{});
        }

        characterController_->pushEventToQueue(RotateTargetEvent{caclulateTargetRotation(vectorToPlayer)});
        return;
    }

    if (movingPoints_.empty())
    {
        characterController_->pushEventToQueue(EndRotationEvent{});
        characterController_->pushEventToQueue(EndForwardMoveEvent{});
        return;
    }

    auto vectorToTarget = freeWalkingTargetPoint - thisObject_.GetWorldTransform().GetPosition();
   // characterController_->pushEventToQueue(EndAttackEvent{});
    characterController_->pushEventToQueue(RotateTargetEvent{caclulateTargetRotation(vectorToTarget)});
    characterController_->pushEventToQueue(MoveForwardEvent{});

    auto distanceToPoint = glm::length(vectorToTarget);
    //DEBUG_LOG("Enemy pos: " + std::to_string(thisObject_.GetWorldTransform().GetPosition()) +
    //          ", freeWalkingTargetPoint=" + std::to_string(freeWalkingTargetPoint) +
    //          ", distance: " + std::to_string(distanceToPoint));

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

    const float range  = generatePathParams.range;
    const float offset = generatePathParams.offset;

    movingPoints_.resize(4);
    movingPoints_[0] = position + vec3(getRandomFloat() * range + offset, 0, getRandomFloat() * range + offset);
    movingPoints_[1] = position + vec3(getRandomFloat() * range + offset, 0, -getRandomFloat() * range - offset);
    movingPoints_[2] = position + vec3(-getRandomFloat() * range - offset, 0, -getRandomFloat() * range - offset);
    movingPoints_[3] = position + vec3(-getRandomFloat() * range - offset, 0, getRandomFloat() * range + offset);

    // To do : // get height of neigbour moving points?
    //for (auto& point : movingPoints_)
    //{
    //    auto h = componentContext_.physicsApi_.RayTest(vec3(point.x, 10000, point.z), vec3(point.x, -10000, point.z));
    //    if (h)
    //    {
    //        point.y = h->pointWorld.y;
    //    }
    //}

    freeWalkingTargetPointIndex = 0;
    freeWalkingTargetPoint      = movingPoints_[freeWalkingTargetPointIndex];
}
void EnemyController::registerReadFunctions()
{
    auto readFunc = [](ComponentContext& componentContext, const TreeNode& node, GameObject& gameObject)
    {
        auto component = std::make_unique<EnemyController>(componentContext, gameObject);

        EnemyController::GeneratePathParams generatePathParams;
        const auto& gmn = node.getChild(CSTR_GENERATE_MOVING_PATH);
        if (gmn)
        {
            const auto& activeNode = gmn->getChild(CSTR_GENERATE_MOVING_PATH_ACTIVE);
            if (activeNode)
            {
                ::Read(activeNode, generatePathParams.isActive);
            }

            const auto& rangeNode = gmn->getChild(CSTR_GENERATE_MOVING_PATH_RANGE);
            if (rangeNode)
            {
                ::Read(rangeNode, generatePathParams.range);
            }

            const auto& offsetNode = gmn->getChild(CSTR_GENERATE_MOVING_PATH_OFFSET);
            if (offsetNode)
            {
                ::Read(offsetNode, generatePathParams.offset);
            }
            component->generatePathParams = generatePathParams;
        }
        return component;
    };
    regsiterComponentReadFunction(COMPONENT_STR, readFunc);
}
void EnemyController::write(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, COMPONENT_STR});

    auto& n = node.addChild(CSTR_GENERATE_MOVING_PATH);
    ::write(n.addChild(CSTR_GENERATE_MOVING_PATH_ACTIVE), generatePathParams.isActive);
    ::write(n.addChild(CSTR_GENERATE_MOVING_PATH_RANGE), generatePathParams.range);
    ::write(n.addChild(CSTR_GENERATE_MOVING_PATH_OFFSET), generatePathParams.offset);
}
}  // namespace Components
}  // namespace GameEngine
