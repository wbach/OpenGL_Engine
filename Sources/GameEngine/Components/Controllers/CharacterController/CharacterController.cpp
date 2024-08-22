#include "CharacterController.h"

#include <algorithm>

#include "AnimationClipNames.h"
#include "CharacterControllerFsm.h"
#include "FsmContext.h"
#include "GameEngine/Components/Camera/ThridPersonCamera/ThridPersonCameraComponent.h"
#include "GameEngine/Components/CommonReadDef.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Components/Controllers/CharacterController/AimController.h"
#include "GameEngine/Components/Physics/CapsuleShape.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Physics/IPhysicsApi.h"
#include "States/AimCrouchRotateState.h"

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
const std::string CSTR_AIM_JOINT_NAME            = "aimJointName";
const std::string CSTR_POSTURE                   = "posture";
const std::string CSTR_MOVEMENT                  = "movement";
const std::string CSTR_POSTURE_STAND             = "stand";
const std::string CSTR_POSTURE_CROUCHED          = "crouched";
const std::string CSTR_ROTATE                    = "rotate";
const std::string CSTR_AIM                       = "aim";

}  // namespace

void write(TreeNode& node, const MovmentClipNames& names)
{
    ::write(node.addChild(CSTR_ANIMATION_FORWARD), names.forward);
    ::write(node.addChild(CSTR_ANIMATION_BACKWARD), names.backward);
    ::write(node.addChild(CSTR_ANIMATION_MOVE_LEFT), names.left);
    ::write(node.addChild(CSTR_ANIMATION_MOVE_RIGHT), names.right);
}

void write(TreeNode& node, const PlayStateType& stateType)
{
    std::string s{"unknown"};
    switch (stateType)
    {
        case PlayStateType::idle:
            s = "idle";
            break;
        case PlayStateType::walk:
            s = "walk";
            break;
        case PlayStateType::run:
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

void write(TreeNode& node, const Movement& movement)
{
    ::write(node.addChild(CSTR_ANIMATION_WALK), movement.walk);
    ::write(node.addChild(CSTR_ANIMATION_RUN), movement.run);
    ::write(node.addChild(CSTR_ANIMATION_CROUCH_MOVEMENT), movement.crouch);
}

void write(TreeNode& node, const RotateAnimationClips& rotateClips)
{
    ::write(node.addChild(CSTR_ANIMATION_ROTATE_LEFT), rotateClips.left);
    ::write(node.addChild(CSTR_ANIMATION_ROTATE_RIGHT), rotateClips.right);
}

void write(TreeNode& node, const PostureClipNames& postureClips)
{
    ::write(node.addChild(CSTR_IDLE_MAIN), postureClips.idle);
    ::write(node.addChild(CSTR_DEATH_ANIMATION), postureClips.death);
    ::write(node.addChild(CSTR_HURT_ANIMATION), postureClips.hurt);
    ::write(node.addChild(CSTR_ROTATE), postureClips.rotate);
}

void write(TreeNode& node, const Posture& posture)
{
    ::write(node.addChild(CSTR_POSTURE_STAND), posture.stand);
    ::write(node.addChild(CSTR_POSTURE_CROUCHED), posture.crouched);
}

void write(TreeNode& node, const StateClipsNames& names)
{
    ::write(node.addChild(CSTR_MOVEMENT), names.movement);
    ::write(node.addChild(CSTR_POSTURE), names.posture);
    ::write(node.addChild(CSTR_JUMP_ANIMATION), names.jump);
    ::write(node.addChild(CSTR_ATTACK_ANIMATIONS), names.attack);
    ::write(node.addChild(CSTR_ANIMATION_SPRINT), names.sprint);
    ::write(node.addChild(CSTR_ANIMATION_DOGE), names.doge);
    ::write(node.addChild(CSTR_ANIMATION_DOGE_JUMP), names.dogeJump);
}

void write(TreeNode& node, const AimClips& names)
{
    ::write(node.addChild(CSTR_DRAW_ARROW_ANIMATION), names.draw);
    ::write(node.addChild(CSTR_RECOIL_ARROW_ANIMATION), names.recoil);
    ::write(node.addChild(CSTR_AIM_IDLE_ANIMATION), names.idle);
}

void write(TreeNode& node, const AnimationClipsNames& names)
{
    ::write(node.addChild(CSTR_ANIMATION_ARMED), names.armed);
    ::write(node.addChild(CSTR_ANIMATION_DISARMED), names.disarmed);

    ::write(node.addChild(CSTR_ANIMATION_EQUIP), names.equip);
    ::write(node.addChild(CSTR_ANIMATION_DISARM), names.disarm);
    ::write(node.addChild(CSTR_AIM), names.aim);
}

void Read(const TreeNode& node, MovmentClipNames& result)
{
    Read(node.getChild(CSTR_ANIMATION_FORWARD), result.forward);
    Read(node.getChild(CSTR_ANIMATION_BACKWARD), result.backward);
    Read(node.getChild(CSTR_ANIMATION_MOVE_LEFT), result.left);
    Read(node.getChild(CSTR_ANIMATION_MOVE_RIGHT), result.right);
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
            result.push_back({node->value_, PlayStateType::idle});
        }
        else
        {
            AttackAnimation aa{"", PlayStateType::idle};
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
                    aa.stateType = PlayStateType::walk;
                }
                if (maybePlayStateTypeNode->value_ == "run")
                {
                    aa.stateType = PlayStateType::run;
                }
            }
            result.push_back(aa);
        }
    }
}
void Read(const TreeNode& node, RotateAnimationClips& result)
{
    Read(node.getChild(CSTR_ANIMATION_ROTATE_LEFT), result.left);
    Read(node.getChild(CSTR_ANIMATION_ROTATE_RIGHT), result.right);
}

void Read(const TreeNode& node, PostureClipNames& result)
{
    Read(node.getChild(CSTR_IDLE_MAIN), result.idle);
    Read(node.getChild(CSTR_ANIMATION_CROUCH_IDLE), result.idle);
    Read(node.getChild(CSTR_HURT_ANIMATION), result.hurt);
    Read(node.getChild(CSTR_DEATH_ANIMATION), result.death);
    Read(node.getChild(CSTR_ROTATE), result.rotate);
}

void Read(const TreeNode& node, Posture& result)
{
    Read(node.getChild(CSTR_POSTURE_STAND), result.stand);
    Read(node.getChild(CSTR_POSTURE_CROUCHED), result.crouched);
}

void Read(const TreeNode& node, Movement& result)
{
    Read(node.getChild(CSTR_ANIMATION_WALK), result.walk);
    Read(node.getChild(CSTR_ANIMATION_RUN), result.run);
    Read(node.getChild(CSTR_ANIMATION_CROUCH_MOVEMENT), result.crouch);
}

void Read(const TreeNode& node, StateClipsNames& result)
{
    Read(node.getChild(CSTR_MOVEMENT), result.movement);
    Read(node.getChild(CSTR_POSTURE), result.posture);
    Read(node.getChild(CSTR_JUMP_ANIMATION), result.jump);
    Read(node.getChild(CSTR_ATTACK_ANIMATIONS), result.attack);
    Read(node.getChild(CSTR_ANIMATION_SPRINT), result.sprint);
    Read(node.getChild(CSTR_ANIMATION_DOGE_JUMP), result.dogeJump);
    Read(node.getChild(CSTR_ANIMATION_DOGE), result.doge);
}

void Read(const TreeNode& node, AimClips& result)
{
    Read(node.getChild(CSTR_DRAW_ARROW_ANIMATION), result.draw);
    Read(node.getChild(CSTR_RECOIL_ARROW_ANIMATION), result.recoil);
    Read(node.getChild(CSTR_AIM_IDLE_ANIMATION), result.idle);
}

void Read(const TreeNode& node, AnimationClipsNames& result)
{
    Read(node.getChild(CSTR_ANIMATION_ARMED), result.armed);
    Read(node.getChild(CSTR_ANIMATION_DISARMED), result.disarmed);

    Read(node.getChild(CSTR_ANIMATION_EQUIP), result.equip);
    Read(node.getChild(CSTR_ANIMATION_DISARM), result.disarm);

    Read(node.getChild(CSTR_AIM), result.aim);
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
    , aimJointName_{"mixamorig:Spine2"}
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
        auto aimJoint = animator_->GetJoint(aimJointName_);
        if (not aimJoint)
            ERROR_LOG("Aim joint not found : " + aimJointName_);

        impl->aimController_ =
            std::make_unique<AimController>(componentContext_.scene_, thisObject_, componentContext_.inputManager_,
                                            aimJoint ? *aimJoint : dummyJoint, animationClipsNames_.aim.draw);

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
            DisarmedAttackAndWalkAndRotateState(context),
            DisarmedRunState(context),
            DisarmedRotateState(context),
            DisarmedCrouchRotateState(context),
            DisarmedRunAndRotateState(context),
            DisarmedWalkState(context),
            DisarmedWalkAndRotateState(context),
            DisarmedCrouchWalkAndRotateState(context),
            DisarmedCrouchWalkState(context),
            DisarmedSprintState(context),
            DisarmedSprintAndRotateState(context),
            IdleArmedChangeState(context),
            IdleCrouchArmedChangeState(context),
            RotateArmedChangeState(context),
            CrouchRotateArmedChangeState(context),
            RunArmedChangeState(context),
            RunAndRotateArmedChangeState(context),
            WalkArmedChangeState(context),
            WalkAndRotateArmedChangeState(context),
            CrouchWalkAndRotateArmedChangeState(context),
            CrouchWalkArmedChangeState(context),
            ArmedIdleState(context),
            ArmedIdleCrouchState(context),
            ArmedAttackState(context),
            ArmedAttackAndRotateState(context),
            ArmedAttackAndRunAndRotateState(context),
            ArmedAttackAndWalkAndRotateState(context),
            ArmedRunState(context),
            ArmedRotateState(context),
            ArmedCrouchRotateState(context),
            ArmedRunAndRotateState(context),
            ArmedWalkState(context),
            ArmedWalkAndRotateState(context),
            ArmedCrouchWalkAndRotateState(context),
            ArmedCrouchWalkState(context),
            ArmedSprintState(context),
            ArmedSprintAndRotateState(context),
            ArmedAttackAndRunState(context),
            ArmedAttackAndWalkState(context),
            JumpState(context),
            AimState(context),
            AimCrouchState(context),
            AimCrouchWalkState(context),
            AimCrouchRotateState(context),
            AimCrouchWalkAndRotateState(context),
            AimRotateState(context),
            AimWalkState(context),
            AimWalkAndRotateState(context),
            RecoilState(context),
            RecoilCrouchState(context),
            RecoilCrouchWalkState(context),
            RecoilRotateState(context),
            RecoilWalkState(context),
            RecoilWalkAndRotateState(context),
            RecoilCrouchRotateState(context),
            RecoilCrouchWalkAndRotateState(context),
            DrawArrowState(context),
            DrawArrowCrouchState(context),
            DrawArrowRotateState(context),
            DrawArrowWalkState(context),
            DrawArrowWalkAndRotateState(context),
            DrawArrowCrouchWalkState(context),
            DrawArrowCrouchRotateState(context),
            DrawArrowCrouchWalkAndRotateState(context),
            DeathState(context));
        // clang-format on

        rigidbody_->SetAngularFactor(0.f);

        animator_->setPlayOnceForAnimationClip(animationClipsNames_.equip);
        animator_->setPlayOnceForAnimationClip(animationClipsNames_.disarm);
        animator_->setPlayOnceForAnimationClip(animationClipsNames_.disarmed.jump);
        animator_->setPlayOnceForAnimationClip(animationClipsNames_.disarmed.posture.stand.hurt);
        animator_->setPlayOnceForAnimationClip(animationClipsNames_.disarmed.posture.stand.death);

        // animator_->alignAnimations(animationClipsNames_.disarm, animationClipsNames_.disarmed.idle);

        for (const auto& attack : animationClipsNames_.armed.attack)
        {
            animator_->setPlayOnceForAnimationClip(attack.name);
        }
        for (const auto& attack : animationClipsNames_.disarmed.attack)
        {
            animator_->setPlayOnceForAnimationClip(attack.name);
        }

        animator_->SetAnimation(animationClipsNames_.disarmed.posture.stand.idle);

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
        ::Read(node.getChild(CSTR_ANIMATION_CLIPS), component->animationClipsNames_);
        ::Read(node.getChild(CSTR_EQUIP_TIMESTAMP), component->equipTimeStamp);
        ::Read(node.getChild(CSTR_DISARM_TIMESTAMP), component->disarmTimeStamp);
        ::Read(node.getChild(CSTR_AIM_JOINT_NAME), component->aimJointName_);

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
    ::write(node.addChild(CSTR_AIM_JOINT_NAME), aimJointName_);
}
}  // namespace Components
}  // namespace GameEngine
