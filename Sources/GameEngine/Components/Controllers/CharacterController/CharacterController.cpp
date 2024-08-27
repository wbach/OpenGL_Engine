#include "CharacterController.h"

#include "AnimationClipNames.h"
#include "CharacterControllerFsm.h"
#include "FsmContext.h"
#include "GameEngine/Components/CommonReadDef.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Components/Controllers/CharacterController/AimController.h"
#include "GameEngine/Components/Physics/CapsuleShape.h"
#include "GameEngine/Objects/GameObject.h"
#include "Serializers/ReadFunctions.h"
#include "Serializers/Variables.h"
#include "Serializers/WriteFunctions.h"
#include "States/AimCrouchRotateState.h"

using namespace GameEngine::Components;
using namespace GameEngine::Components::Character;

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
        Character::Read(node.getChild(CSTR_ANIMATION_CLIPS), component->animationClipsNames_);
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

    Character::write(node.addChild(CSTR_ANIMATION_CLIPS), animationClipsNames_);
    ::write(node.addChild(CSTR_EQUIP_TIMESTAMP), equipTimeStamp);
    ::write(node.addChild(CSTR_DISARM_TIMESTAMP), disarmTimeStamp);
    ::write(node.addChild(CSTR_AIM_JOINT_NAME), aimJointName_);
}
}  // namespace Components
}  // namespace GameEngine
