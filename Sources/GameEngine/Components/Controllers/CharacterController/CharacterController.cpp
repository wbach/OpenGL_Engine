#include "CharacterController.h"

#include <algorithm>

#include "CharacterControllerFsm.h"
#include "FsmContext.h"
#include "GameEngine/Components/Camera/ThridPersonCamera/ThridPersonCameraComponent.h"
#include "GameEngine/Components/CommonReadDef.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Components/Controllers/CharacterController/AimController.h"
#include "GameEngine/Components/Physics/CapsuleShape.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Physics/IPhysicsApi.h"

using namespace GameEngine::Components;

namespace
{
const std::string COMPONENT_STR = "CharacterController";

const std::string CSTR_EQUIP_TIMESTAMP  = "equipTimeStamp";
const std::string CSTR_DISARM_TIMESTAMP = "disarmTimeStamp";

const std::string CSTR_ATTACK_ANIMATIONS         = "attackAnimations";
const std::string CSTR_ANIMATION                 = "animation";
const std::string CSTR_ANIMATION_STATE_TYPE      = "stateType";
const std::string CSTR_ANIMATION_ARMED           = "armedAnimations";
const std::string CSTR_ANIMATION_DISARMED        = "disarmedAnimations";
const std::string CSTR_ANIMATION_CROUCH          = "crouchAnimations";
const std::string CSTR_ANIMATION_EQUIP           = "equipAnimation";
const std::string CSTR_ANIMATION_DISARM          = "disarmAnimation";
const std::string CSTR_ANIMATION_WALK            = "walk";
const std::string CSTR_ANIMATION_RUN             = "run";
const std::string CSTR_ANIMATION_SPRINT          = "sprint";
const std::string CSTR_ANIMATION_CROUCH_IDLE     = "crouchIdle";
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
const std::string CSTR_DRAW_ARROW_ANIMATION      = "drawArrow";
const std::string CSTR_RECOIL_ARROW_ANIMATION    = "recoilArrow";
const std::string CSTR_AIM_IDLE_ANIMATION        = "aimIdle";

}  // namespace

void write(TreeNode& node, const MovmentClipNames& names)
{
    ::write(node.addChild(CSTR_ANIMATION_FORWARD), names.forward);
    ::write(node.addChild(CSTR_ANIMATION_BACKWARD), names.backward);
    ::write(node.addChild(CSTR_ANIMATION_MOVE_LEFT), names.moveleft);
    ::write(node.addChild(CSTR_ANIMATION_MOVE_RIGHT), names.moveRight);
}

void write(TreeNode& node, const AttackAnimation::PlayStateType& stateType)
{
    std::string s{"unknown"};
    switch (stateType)
    {
        case AttackAnimation::PlayStateType::idle:
            s = "idle";
            break;
        case AttackAnimation::PlayStateType::walk:
            s = "walk";
            break;
        case AttackAnimation::PlayStateType::run:
            s = "run";
            break;
    }
    ::write(node, s);
}

void write(TreeNode& node, const std::vector<AttackAnimation>& names)
{
    for (const auto& anim : names)
    {
        auto& n = node.addChild(CSTR_ANIMATION);

        ::write(n.addChild(CSTR_NAME), anim.name);
        ::write(n.addChild(CSTR_ANIMATION_STATE_TYPE), anim.stateType);
    }
}

void write(TreeNode& node, const StateClipsNames& names)
{
    ::write(node.addChild(CSTR_ANIMATION_WALK), names.walk);
    ::write(node.addChild(CSTR_ANIMATION_RUN), names.run);
    ::write(node.addChild(CSTR_ANIMATION_CROUCH_MOVEMENT), names.crouch);
    ::write(node.addChild(CSTR_JUMP_ANIMATION), names.jump);
    ::write(node.addChild(CSTR_IDLE_MAIN), names.idle);
    ::write(node.addChild(CSTR_ANIMATION_CROUCH_IDLE), names.crouchIdle);
    ::write(node.addChild(CSTR_HURT_ANIMATION), names.hurt);
    ::write(node.addChild(CSTR_DEATH_ANIMATION), names.death);
    ::write(node.addChild(CSTR_ATTACK_ANIMATIONS), names.attack);
    ::write(node.addChild(CSTR_ANIMATION_ROTATE_LEFT), names.rotateLeft);
    ::write(node.addChild(CSTR_ANIMATION_ROTATE_RIGHT), names.rotateRight);
    ::write(node.addChild(CSTR_ANIMATION_SPRINT), names.sprint);
}

void write(TreeNode& node, const AnimationClipsNames& names)
{
    ::write(node.addChild(CSTR_ANIMATION_ARMED), names.armed);
    ::write(node.addChild(CSTR_ANIMATION_DISARMED), names.disarmed);
    ::write(node.addChild(CSTR_ANIMATION_DOGE), names.doge);
    ::write(node.addChild(CSTR_ANIMATION_DOGE_JUMP), names.dogeJump);
    ::write(node.addChild(CSTR_ANIMATION_EQUIP), names.equip);
    ::write(node.addChild(CSTR_ANIMATION_DISARM), names.disarm);
    ::write(node.addChild(CSTR_DRAW_ARROW_ANIMATION), names.drawArrow);
    ::write(node.addChild(CSTR_RECOIL_ARROW_ANIMATION), names.recoilArrow);
    ::write(node.addChild(CSTR_AIM_IDLE_ANIMATION), names.aimIdle);
}

void Read(const TreeNode& node, MovmentClipNames& result)
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

void Read(const TreeNode& node, std::vector<AttackAnimation>& result)
{
    for (const auto& node : node.getChildren())
    {
        if (!node->value_.empty())
        {
            result.push_back({node->value_, AttackAnimation::PlayStateType::idle});
        }
        else
        {
            AttackAnimation aa{"", AttackAnimation::PlayStateType::idle};
            auto maybeNameNode = node->getChild(CSTR_NAME);
            if (maybeNameNode)
            {
                Read(*maybeNameNode, aa.name);
            }
            auto maybePlayStateTypeNode = node->getChild(CSTR_ANIMATION_STATE_TYPE);
            if (maybePlayStateTypeNode)
            {
                if (maybePlayStateTypeNode->value_ == "walk")
                {
                    aa.stateType = AttackAnimation::PlayStateType::walk;
                }
                if (maybePlayStateTypeNode->value_ == "run")
                {
                    aa.stateType = AttackAnimation::PlayStateType::run;
                }
            }
            result.push_back(aa);
        }
    }
}

void Read(const TreeNode& node, StateClipsNames& result)
{
    Read(node.getChild(CSTR_ANIMATION_WALK), result.walk);
    Read(node.getChild(CSTR_ANIMATION_RUN), result.run);
    Read(node.getChild(CSTR_ANIMATION_CROUCH_MOVEMENT), result.crouch);
    Read(node.getChild(CSTR_JUMP_ANIMATION), result.jump);
    Read(node.getChild(CSTR_IDLE_MAIN), result.idle);
    Read(node.getChild(CSTR_ANIMATION_CROUCH_IDLE), result.crouchIdle);
    Read(node.getChild(CSTR_HURT_ANIMATION), result.hurt);
    Read(node.getChild(CSTR_DEATH_ANIMATION), result.death);
    Read(node.getChild(CSTR_ATTACK_ANIMATIONS), result.attack);
    Read(node.getChild(CSTR_ANIMATION_ROTATE_LEFT), result.rotateLeft);
    Read(node.getChild(CSTR_ANIMATION_ROTATE_RIGHT), result.rotateRight);
    Read(node.getChild(CSTR_ANIMATION_SPRINT), result.sprint);
}

void Read(const TreeNode& node, AnimationClipsNames& result)
{
    Read(node.getChild(CSTR_ANIMATION_ARMED), result.armed);
    Read(node.getChild(CSTR_ANIMATION_DISARMED), result.disarmed);
    Read(node.getChild(CSTR_ANIMATION_DOGE), result.doge);
    Read(node.getChild(CSTR_ANIMATION_DOGE_JUMP), result.dogeJump);
    Read(node.getChild(CSTR_ANIMATION_EQUIP), result.equip);
    Read(node.getChild(CSTR_ANIMATION_DISARM), result.disarm);
    Read(node.getChild(CSTR_DRAW_ARROW_ANIMATION), result.drawArrow);
    Read(node.getChild(CSTR_RECOIL_ARROW_ANIMATION), result.recoilArrow);
    Read(node.getChild(CSTR_AIM_IDLE_ANIMATION), result.aimIdle);
}

namespace GameEngine
{
namespace Components
{
namespace
{
Animation::Joint dummyJoint;
}

struct CharacterController::Impl
{
    std::unique_ptr<FsmContext> fsmContext;
    std::unique_ptr<CharacterControllerFsm> stateMachine_;
    std::unique_ptr<AimController> aimController_;

    void CleanUp()
    {
        stateMachine_.reset();
        fsmContext.reset();
    }
};

CharacterController::CharacterController(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(typeid(CharacterController).hash_code(), componentContext, gameObject)
    , upperBodyGroupName{"upperBody"}
    , lowerBodyGroupName{"lowerBody"}
    , equipTimeStamp{-1.0}
    , disarmTimeStamp{-1.0}
    , rigidbody_{nullptr}
    , shapeSize_(1.f)
{
    impl = std::make_unique<CharacterController::Impl>();
}

CharacterController::~CharacterController()
{
}
void CharacterController::CleanUp()
{
    impl->CleanUp();
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
        auto aimJoint = animator_->GetJoint("mixamorig:Spine2");
        if (not aimJoint)
            ERROR_LOG("Aim joint not found");

        impl->aimController_ = std::make_unique<AimController>(
            componentContext_.scene_, thisObject_, componentContext_.inputManager_, aimJoint ? *aimJoint : dummyJoint);

        impl->fsmContext.reset(new FsmContext{thisObject_,
                                              componentContext_.physicsApi_,
                                              *rigidbody_,
                                              *animator_,
                                              *this,
                                              componentContext_.inputManager_,
                                              *impl->aimController_,
                                              impl->stateMachine_,
                                              {},
                                              {},
                                              animationClipsNames_,
                                              upperBodyGroupName,
                                              lowerBodyGroupName,
                                              {equipTimeStamp, disarmTimeStamp},
                                              moveSpeeds_.walkSpeed,
                                              moveSpeeds_.runSpeed,
                                              moveSpeeds_.crouchSpeed,
                                              moveSpeeds_.sprintSpeed});
        auto& context = *impl->fsmContext;
        // clang-format off
        impl->stateMachine_ = std::make_unique<CharacterControllerFsm>(
            DisarmedIdleState(context),
            DisarmedIdleCrouchState(context),
            DisarmedAttackState(context),
            DisarmedAttackAndRunState(context),
            DisarmedAttackAndWalkState(context),
            DisarmedAttackAndRotateState(context),
            DisarmedAttackAndRunAndRotateState(context),
            DisarmedRunState(context),
            DisarmedRotateState(context),
            DisarmedRunAndRotateState(context),
            DisarmedWalkState(context),
            DisarmedWalkAndRotateState(context),
            DisarmedCrouchWalkState(context),
            DisarmedSprintState(context),
            DisarmedSprintAndRotateState(context),
            IdleArmedChangeState(context),
            IdleCrouchArmedChangeState(context),
            RotateArmedChangeState(context),
            RunArmedChangeState(context),
            RunAndRotateArmedChangeState(context),
            WalkArmedChangeState(context),
            WalkAndRotateArmedChangeState(context),
            CrouchWalkArmedChangeState(context),
            ArmedIdleState(context),
            ArmedIdleCrouchState(context),
            ArmedAttackState(context),
            ArmedRunState(context),
            ArmedRotateState(context),
            ArmedRunAndRotateState(context),
            ArmedWalkState(context),
            ArmedWalkAndRotateState(context),
            ArmedCrouchWalkState(context),
            ArmedSprintState(context),
            ArmedSprintAndRotateState(context),
            JumpState(context, [&]() { impl->stateMachine_->handle(EndJumpEvent{}); }),
            MoveJumpState(context, [&]() { impl->stateMachine_->handle(EndJumpEvent{}); }),
            AimState(context),
            AimCrouchState(context),
            AimRotateState(context),
            AimWalkState(context),
            AimWalkAndRotateState(context),
            RecoilState(context),
            RecoilCrouchState(context),
            RecoilRotateState(context),
            RecoilWalkState(context),
            RecoilWalkAndRotateState(context),
            DrawArrowState(context),
            DrawArrowCrouchState(context),
            DrawArrowRotateState(context),
            DrawArrowWalkState(context),
            DrawArrowWalkAndRotateState(context),
            DeathState(context));
        // clang-format on

        rigidbody_->SetAngularFactor(0.f);

        animator_->setPlayOnceForAnimationClip(animationClipsNames_.equip);
        animator_->setPlayOnceForAnimationClip(animationClipsNames_.disarm);
        animator_->setPlayOnceForAnimationClip(animationClipsNames_.disarmed.jump);
        animator_->setPlayOnceForAnimationClip(animationClipsNames_.disarmed.hurt);
        animator_->setPlayOnceForAnimationClip(animationClipsNames_.disarmed.death);

        // animator_->alignAnimations(animationClipsNames_.disarm, animationClipsNames_.disarmed.idle);

        for (const auto& attack : animationClipsNames_.armed.attack)
        {
            animator_->setPlayOnceForAnimationClip(attack.name);
        }
        for (const auto& attack : animationClipsNames_.disarmed.attack)
        {
            animator_->setPlayOnceForAnimationClip(attack.name);
        }

        animator_->SetAnimation(animationClipsNames_.disarmed.idle);

        auto lowerBodyGroupIter = animator_->jointGroups_.find(lowerBodyGroupName);
        if (lowerBodyGroupIter == animator_->jointGroups_.end())
        {
            // /*DISABLED*/ DEBUG_LOG("lowerBodyGroupName which is : " + lowerBodyGroupName + ", not found in animator,
            // create empty.");
            animator_->jointGroups_.insert({lowerBodyGroupName, {}});
        }

        auto upperBodyGroupIter = animator_->jointGroups_.find(upperBodyGroupName);
        if (upperBodyGroupIter == animator_->jointGroups_.end())
        {
            // /*DISABLED*/ DEBUG_LOG("upperBodyGroupName which is : " + upperBodyGroupName + ", not found in animator,
            // create empty");
            animator_->jointGroups_.insert({upperBodyGroupName, {}});
        }
    }
    else
    {
        WARNING_LOG("Animator or rigidbody_ not exist in object");
    }
}
void CharacterController::processEvent()
{
    if (not eventQueue.empty())
    {
        auto tmpEvents = std::move(eventQueue);
        for (auto& event : tmpEvents)
        {
            std::visit(
                [&](const auto& e)
                {
                    // /*DISABLED*/ DEBUG_LOG("Process event : " + typeName(e));
                    impl->stateMachine_->handle(e);
                },
                event);
        }
    }
}
void CharacterController::Update()
{
    processEvent();

    if (impl->stateMachine_ and rigidbody_ and rigidbody_->IsReady())
    {
        auto passEventToState = [&](auto statePtr)
        {
            // // /*DISABLED*/ DEBUG_LOG("[" + typeName(statePtr) + "] Update dt = " +
            // std::to_string(componentContext_.time_.deltaTime));
            statePtr->update(componentContext_.time_.deltaTime);
        };
        std::visit(passEventToState, impl->stateMachine_->currentState);
    }
}
void CharacterController::handleEvent(const CharacterControllerEvent& event)
{
    auto passEventToMachine = [&](const auto& e) { impl->stateMachine_->handle(e); };
    std::visit(passEventToMachine, event);
}
float CharacterController::getShapeSize() const
{
    return shapeSize_;
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
