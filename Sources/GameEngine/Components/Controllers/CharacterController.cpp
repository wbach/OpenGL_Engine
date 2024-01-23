#include "CharacterController.h"

#include <algorithm>

#include "GameEngine/Components/CommonReadDef.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Components/Physics/CapsuleShape.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Physics/IPhysicsApi.h"

namespace
{
const std::string COMPONENT_STR = "CharacterController";

const std::string CSTR_EQUIP_TIMESTAMP  = "equipTimeStamp";
const std::string CSTR_DISARM_TIMESTAMP = "disarmTimeStamp";

const std::string CSTR_ATTACK_ANIMATIONS         = "attackAnimations";
const std::string CSTR_ANIMATION                 = "animation";
const std::string CSTR_ANIMATION_ARMED           = "armedAnimations";
const std::string CSTR_ANIMATION_DISARMED        = "disarmedAnimations";
const std::string CSTR_ANIMATION_CROUCH          = "crouchAnimations";
const std::string CSTR_ANIMATION_EQUIP           = "equipAnimation";
const std::string CSTR_ANIMATION_DISARM          = "disarmAnimation";
const std::string CSTR_ANIMATION_WALK            = "walk";
const std::string CSTR_ANIMATION_RUN             = "run";
const std::string CSTR_ANIMATION_SPRINT          = "sprint";
const std::string CSTR_ANIMATION_DOGE            = "doge";
const std::string CSTR_ANIMATION_DOGE_JUMP       = "dogeJump";
const std::string CSTR_ANIMATION_CROUCH_MOVEMENT = "crouchMovement";
const std::string CSTR_ANIMATION_FORWARD         = "forward";
const std::string CSTR_ANIMATION_BACKWARD        = "backward";
const std::string CSTR_ANIMATION_ROTATE_LEFT     = "rotateLeft";
const std::string CSTR_ANIMATION_ROTATE_RIGHT    = "rotateRight";
const std::string CSTR_ANIMATION_MOVE_LEFT       = "moveLeft";
const std::string CSTR_ANIMATION_MOVE_RIGHT      = "moveRight";
const std::string CSTR_JUMP_ANIMATION            = "jump";
const std::string CSTR_IDLE_MAIN                 = "mainIdle";
const std::string CSTR_HURT_ANIMATION            = "hurt";
const std::string CSTR_DEATH_ANIMATION           = "death";
}  // namespace

void write(TreeNode& node, const GameEngine::Components::MovmentClipNames& names)
{
    ::write(node.addChild(CSTR_ANIMATION_FORWARD), names.forward);
    ::write(node.addChild(CSTR_ANIMATION_BACKWARD), names.backward);
    ::write(node.addChild(CSTR_ANIMATION_MOVE_LEFT), names.moveleft);
    ::write(node.addChild(CSTR_ANIMATION_MOVE_RIGHT), names.moveRight);
}
void write(TreeNode& node, const std::vector<std::string>& names)
{
    for (const auto& name : names)
    {
        ::write(node.addChild(CSTR_ANIMATION), name);
    }
}

void write(TreeNode& node, const GameEngine::Components::StateClipsNames& names)
{
    ::write(node.addChild(CSTR_ANIMATION_WALK), names.walk);
    ::write(node.addChild(CSTR_ANIMATION_RUN), names.run);
    ::write(node.addChild(CSTR_ANIMATION_CROUCH_MOVEMENT), names.crouch);
    ::write(node.addChild(CSTR_JUMP_ANIMATION), names.jump);
    ::write(node.addChild(CSTR_IDLE_MAIN), names.idle);
    ::write(node.addChild(CSTR_HURT_ANIMATION), names.hurt);
    ::write(node.addChild(CSTR_DEATH_ANIMATION), names.death);
    ::write(node.addChild(CSTR_ATTACK_ANIMATIONS), names.attack);
    ::write(node.addChild(CSTR_ANIMATION_ROTATE_LEFT), names.rotateLeft);
    ::write(node.addChild(CSTR_ANIMATION_ROTATE_RIGHT), names.rotateRight);
}

void write(TreeNode& node, const GameEngine::Components::AnimationClipsNames& names)
{
    ::write(node.addChild(CSTR_ANIMATION_ARMED), names.armed);
    ::write(node.addChild(CSTR_ANIMATION_DISARMED), names.disarmed);
    ::write(node.addChild(CSTR_ANIMATION_SPRINT), names.sprint);
    ::write(node.addChild(CSTR_ANIMATION_DOGE), names.doge);
    ::write(node.addChild(CSTR_ANIMATION_DOGE_JUMP), names.dogeJump);
    ::write(node.addChild(CSTR_ANIMATION_EQUIP), names.equip);
    ::write(node.addChild(CSTR_ANIMATION_DISARM), names.disarm);
}

void Read(const TreeNode& node, GameEngine::Components::MovmentClipNames& result)
{
    Read(node.getChild(CSTR_ANIMATION_FORWARD), result.forward);
    Read(node.getChild(CSTR_ANIMATION_BACKWARD), result.backward);
    Read(node.getChild(CSTR_ANIMATION_MOVE_LEFT), result.moveleft);
    Read(node.getChild(CSTR_ANIMATION_MOVE_RIGHT), result.moveRight);
}

void Read(const TreeNode& node, std::vector<std::string>& result)
{
    for (const auto& node : node.getChildren())
    {
        std::string str;
        Read(*node, str);
        result.push_back(str);
    }
}

void Read(const TreeNode& node, GameEngine::Components::StateClipsNames& result)
{
    Read(node.getChild(CSTR_ANIMATION_WALK), result.walk);
    Read(node.getChild(CSTR_ANIMATION_RUN), result.run);
    Read(node.getChild(CSTR_ANIMATION_CROUCH_MOVEMENT), result.crouch);
    Read(node.getChild(CSTR_JUMP_ANIMATION), result.jump);
    Read(node.getChild(CSTR_IDLE_MAIN), result.idle);
    Read(node.getChild(CSTR_HURT_ANIMATION), result.hurt);
    Read(node.getChild(CSTR_DEATH_ANIMATION), result.death);
    Read(node.getChild(CSTR_ATTACK_ANIMATIONS), result.attack);
    Read(node.getChild(CSTR_ANIMATION_ROTATE_LEFT), result.rotateLeft);
    Read(node.getChild(CSTR_ANIMATION_ROTATE_RIGHT), result.rotateRight);
}

void Read(const TreeNode& node, GameEngine::Components::AnimationClipsNames& result)
{
    Read(node.getChild(CSTR_ANIMATION_ARMED), result.armed);
    Read(node.getChild(CSTR_ANIMATION_DISARMED), result.disarmed);
    Read(node.getChild(CSTR_ANIMATION_SPRINT), result.sprint);
    Read(node.getChild(CSTR_ANIMATION_DOGE), result.doge);
    Read(node.getChild(CSTR_ANIMATION_DOGE_JUMP), result.dogeJump);
    Read(node.getChild(CSTR_ANIMATION_EQUIP), result.equip);
    Read(node.getChild(CSTR_ANIMATION_DISARM), result.disarm);
}

namespace GameEngine
{
namespace Components
{

CharacterController::CharacterController(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(typeid(CharacterController).hash_code(), componentContext, gameObject)
    , upperBodyGroupName{"upperBody"}
    , lowerBodyGroupName{"lowerBody"}
    , equipTimeStamp{-1.0}
    , disarmTimeStamp{-1.0}
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
    RegisterFunction(FunctionType::OnStart, std::bind(&CharacterController::Init, this));
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
        auto sendEndAtatackCallback = [this]()
        {
            if (stateMachine_)
                stateMachine_->handle(EndAttackEvent{});
        };

        attackFsmContext.reset(
            new AttackFsmContext{*animator_, animationClipsNames_, sendEndAtatackCallback, std::nullopt});

        attackFsm_ = std::make_unique<AttackFsm>(EmptyState(), AttackState(*attackFsmContext));

        fsmContext.reset(new FsmContext{*attackFsm_,
                                        thisObject_,
                                        componentContext_.physicsApi_,
                                        *rigidbody_,
                                        *animator_,
                                        *this,
                                        {},
                                        {},
                                        animationClipsNames_,
                                        upperBodyGroupName,
                                        lowerBodyGroupName,
                                        {equipTimeStamp, disarmTimeStamp}});
        // clang-format off

        stateMachine_ = std::make_unique<CharacterControllerFsm>(
            DisarmedIdleState(*fsmContext),
            DisarmedRunState(*fsmContext),
            DisarmedRotateState(*fsmContext),
            DisarmedRunAndRotateState(*fsmContext),
            DisarmedWalkState(*fsmContext),
            DisarmedWalkAndRotateState(*fsmContext),
            ArmedIdleState(*fsmContext),
            ArmedRunState(*fsmContext),
            ArmedRotateState(*fsmContext),
            ArmedRunAndRotateState(*fsmContext),
            ArmedWalkState(*fsmContext),
            ArmedWalkAndRotateState(*fsmContext),
            JumpState(*fsmContext, [&]() { stateMachine_->handle(EndJumpEvent{}); }),
            MoveJumpState(*fsmContext, [&]() { stateMachine_->handle(EndJumpEvent{}); }),
            DeathState(*fsmContext));
        // clang-format on

        rigidbody_->InputParams().angularFactor_ = vec3(0);
        animator_->setPlayOnceForAnimationClip(animationClipsNames_.equip);
        animator_->setPlayOnceForAnimationClip(animationClipsNames_.disarm);
        animator_->setPlayOnceForAnimationClip(animationClipsNames_.disarmed.jump);
        animator_->setPlayOnceForAnimationClip(animationClipsNames_.disarmed.hurt);
        animator_->setPlayOnceForAnimationClip(animationClipsNames_.disarmed.death);

        // animator_->alignAnimations(animationClipsNames_.disarm, animationClipsNames_.disarmed.idle);

        for (const auto& attack : animationClipsNames_.armed.attack)
        {
            animator_->setPlayOnceForAnimationClip(attack);
        }
        for (const auto& attack : animationClipsNames_.disarmed.attack)
        {
            animator_->setPlayOnceForAnimationClip(attack);
        }

        animator_->SetAnimation(animationClipsNames_.disarmed.idle);

        auto lowerBodyGroupIter = animator_->jointGroups_.find(lowerBodyGroupName);
        if (lowerBodyGroupIter == animator_->jointGroups_.end())
        {
            DEBUG_LOG("lowerBodyGroupName which is : " + lowerBodyGroupName + ", not found in animator, create empty." );
            animator_->jointGroups_.insert({lowerBodyGroupName, {}});
        }

        auto upperBodyGroupIter = animator_->jointGroups_.find(upperBodyGroupName);
        if (upperBodyGroupIter == animator_->jointGroups_.end())
        {
            DEBUG_LOG("upperBodyGroupName which is : " + upperBodyGroupName + ", not found in animator, create empty" );
            animator_->jointGroups_.insert({upperBodyGroupName, {}});
        }
    }
    else
    {
        WARNING_LOG("Animator or rigidbody_ not exist in object");
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
    auto readFunc = [](ComponentContext& componentContext, const TreeNode& node, GameObject& gameObject)
    {
        auto component = std::make_unique<CharacterController>(componentContext, gameObject);

        auto animationClipsNode = node.getChild(CSTR_ANIMATION_CLIPS);
        if (animationClipsNode)
        {
            ::Read(*animationClipsNode, component->animationClipsNames_);
        }

        ::Read(node.getChild(CSTR_EQUIP_TIMESTAMP), component->equipTimeStamp);
        ::Read(node.getChild(CSTR_DISARM_TIMESTAMP), component->disarmTimeStamp);

        return component;
    };

    regsiterComponentReadFunction(COMPONENT_STR, readFunc);
}

void CharacterController::write(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, COMPONENT_STR});

    ::write(node.addChild(CSTR_ANIMATION_CLIPS), animationClipsNames_);
    ::write(node.addChild(CSTR_EQUIP_TIMESTAMP), equipTimeStamp);
    ::write(node.addChild(CSTR_DISARM_TIMESTAMP), disarmTimeStamp);
}
}  // namespace Components
}  // namespace GameEngine
