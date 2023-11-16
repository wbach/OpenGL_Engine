#include "CharacterController.h"

#include <algorithm>

#include "GameEngine/Components/CommonReadDef.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Components/Physics/CapsuleShape.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Physics/IPhysicsApi.h"

namespace GameEngine
{
namespace Components
{
namespace
{
const std::string COMPONENT_STR               = "CharacterController";
const std::string CSTR_IDLE_ANIMATION         = "idle";
const std::string CSTR_RUN_ANIMATION          = "run";
const std::string CSTR_MOVEBACKWARD_ANIMATION = "moveBackward";
const std::string CSTR_WALK_ANIMATION         = "walk";
const std::string CSTR_ATTACK_ANIMATION       = "attack";
const std::string CSTR_ATTACK_ANIMATION2      = "attack2";
const std::string CSTR_ATTACK_ANIMATION3      = "attack3";
const std::string CSTR_HURT_ANIMATION         = "hurt";
const std::string CSTR_DEATH_ANIMATION        = "death";
const std::string CSTR_JUMP_ANIMATION         = "jump";
}  // namespace
CharacterController::CharacterController(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(typeid(CharacterController).hash_code(), componentContext, gameObject)
    , hurtAnimationName{"Hurt"}
    , attackAnimationName{"Attack"}
    , deathAnimationName{"Death"}
    , jumpAnimationName{"Jump"}
    , moveForwardAnimationName{"Run"}
    , moveBackwardAnimationName{}
    , idleAnimationName{"Idle"}
    , upperBodyGroupName{"upperBody"}
    , lowerBodyGroupName{"lowerBody"}
    , rigidbody_{nullptr}
    , jumpPower_(DEFAULT_JUMP_POWER)
    , turnSpeed_(DEFAULT_TURN_SPEED)
    , runSpeed_(DEFAULT_RUN_SPEED)
    , shapeSize_(1.f)
{
}
void CharacterController::CleanUp()
{
    attackFsm_.reset();
    stateMachine_.reset();
    attackFsmContext.reset();
    fsmContext.reset();
}
void CharacterController::ReqisterFunctions()
{
    RegisterFunction(FunctionType::Awake, std::bind(&CharacterController::Init, this));
    RegisterFunction(FunctionType::Update, std::bind(&CharacterController::Update, this));
}
void CharacterController::Init()
{
    rigidbody_ = thisObject_.GetComponent<Rigidbody>();
    animator_  = thisObject_.GetComponent<Animator>();

    if (auto capsuleShape = thisObject_.GetComponent<CapsuleShape>())
    {
        const auto& scale = thisObject_.GetWorldTransform().GetScale();
        shapeSize_        = capsuleShape->GetRadius() * glm::compMax(vec2(scale.x, scale.z));
    }

    if (animator_ and rigidbody_)
    {
        auto sendEndAtatackCallback = [this]() {
            if (stateMachine_)
                stateMachine_->handle(EndAttackEvent{});
        };


        attackFsmContext.reset(new AttackFsmContext{*animator_,
                                                    {attackAnimationName, attackAnimationName2, attackAnimationName3},
                                                    sendEndAtatackCallback, std::nullopt});

        attackFsm_ = std::make_unique<AttackFsm>(EmptyState(), AttackState(*attackFsmContext));

        fsmContext.reset(new FsmContext{*attackFsm_, thisObject_, componentContext_.physicsApi_, *rigidbody_,
                                        *animator_, moveForwardAnimationName, moveBackwardAnimationName,
                                        jumpAnimationName, idleAnimationName, deathAnimationName, upperBodyGroupName,
                                        lowerBodyGroupName});

        stateMachine_ = std::make_unique<CharacterControllerFsm>(
            IdleState(*fsmContext), MoveState(*fsmContext), RotateState(*fsmContext), MoveAndRotateState(*fsmContext),
            JumpState(*fsmContext, [&]() { stateMachine_->handle(EndJumpEvent{}); }),
            MoveJumpState(*fsmContext, [&]() { stateMachine_->handle(EndJumpEvent{}); }), DeathState(*fsmContext));

        rigidbody_->InputParams().angularFactor_ = vec3(0);
        animator_->setPlayOnceForAnimationClip(jumpAnimationName);
        animator_->setPlayOnceForAnimationClip(hurtAnimationName);
        animator_->setPlayOnceForAnimationClip(deathAnimationName);
        animator_->setPlayOnceForAnimationClip(attackAnimationName);
        animator_->setPlayOnceForAnimationClip(attackAnimationName2);
        animator_->setPlayOnceForAnimationClip(attackAnimationName3);
    }
}
void CharacterController::Update()
{
    if (stateMachine_ and rigidbody_ and rigidbody_->IsReady())
    {
        auto passEventToState = [&](auto statePtr) { statePtr->update(componentContext_.time_.deltaTime); };
        std::visit(passEventToState, stateMachine_->getCurrentState());
    }
}
void CharacterController::SetJumpPower(float v)
{
    jumpPower_ = v;
}
CharacterControllerFsm* CharacterController::fsm()
{
    return stateMachine_.get();
}

float CharacterController::getShapeSize() const
{
    return shapeSize_;
}
void CharacterController::SetTurnSpeed(float v)
{
    turnSpeed_ = v;
}
void CharacterController::SetRunSpeed(float v)
{
    runSpeed_ = v;
}
void CharacterController::registerReadFunctions()
{
    auto readFunc = [](ComponentContext& componentContext, const TreeNode& node, GameObject& gameObject) {
        auto component = std::make_unique<CharacterController>(componentContext, gameObject);

        auto animationClipsNode = node.getChild(CSTR_ANIMATION_CLIPS);
        if (animationClipsNode)
        {
            ::Read(animationClipsNode->getChild(CSTR_IDLE_ANIMATION), component->idleAnimationName);
            ::Read(animationClipsNode->getChild(CSTR_HURT_ANIMATION), component->hurtAnimationName);
            ::Read(animationClipsNode->getChild(CSTR_RUN_ANIMATION), component->moveForwardAnimationName);
            ::Read(animationClipsNode->getChild(CSTR_MOVEBACKWARD_ANIMATION), component->moveBackwardAnimationName);
            ::Read(animationClipsNode->getChild(CSTR_DEATH_ANIMATION), component->deathAnimationName);
            ::Read(animationClipsNode->getChild(CSTR_ATTACK_ANIMATION), component->attackAnimationName);
            ::Read(animationClipsNode->getChild(CSTR_ATTACK_ANIMATION2), component->attackAnimationName2);
            ::Read(animationClipsNode->getChild(CSTR_ATTACK_ANIMATION3), component->attackAnimationName3);
            ::Read(animationClipsNode->getChild(CSTR_JUMP_ANIMATION), component->jumpAnimationName);
        }
        return component;
    };

    regsiterComponentReadFunction(COMPONENT_STR, readFunc);
}
void CharacterController::write(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, COMPONENT_STR});
    auto& animClipsNode = node.addChild(CSTR_ANIMATION_CLIPS);
    ::write(animClipsNode.addChild(CSTR_IDLE_ANIMATION), idleAnimationName);
    ::write(animClipsNode.addChild(CSTR_HURT_ANIMATION), hurtAnimationName);
    ::write(animClipsNode.addChild(CSTR_RUN_ANIMATION), moveForwardAnimationName);
    ::write(animClipsNode.addChild(CSTR_MOVEBACKWARD_ANIMATION), moveBackwardAnimationName);
    ::write(animClipsNode.addChild(CSTR_DEATH_ANIMATION), deathAnimationName);
    ::write(animClipsNode.addChild(CSTR_ATTACK_ANIMATION), attackAnimationName);
    ::write(animClipsNode.addChild(CSTR_ATTACK_ANIMATION2), attackAnimationName2);
    ::write(animClipsNode.addChild(CSTR_ATTACK_ANIMATION3), attackAnimationName3);
    ::write(animClipsNode.addChild(CSTR_JUMP_ANIMATION), jumpAnimationName);
}
}  // namespace Components
}  // namespace GameEngine
