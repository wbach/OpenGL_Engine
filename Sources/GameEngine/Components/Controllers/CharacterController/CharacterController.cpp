#include "CharacterController.h"

#include "AnimationClipNames.h"
#include "CharacterControllerFsm.h"
#include "FsmContext.h"
#include "GameEngine/Components/CommonReadDef.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Components/Controllers/CharacterController/AimController.h"
#include "GameEngine/Components/Physics/CapsuleShape.h"
#include "GameEngine/Components/Physics/SphereShape.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Physics/CollisionContactInfo.h"
#include "Logger/Log.h"
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
}  // namespace

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
    : BaseComponent(GetComponentType<CharacterController>(), componentContext, gameObject)
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
    if (groundEnterSubId)
    {
        componentContext_.physicsApi_.celarCollisionCallback(groundEnterSubId);
    }
    if (groundExitSubId)
    {
        componentContext_.physicsApi_.celarCollisionCallback(groundExitSubId);
    }
    isInit = false;
}
void CharacterController::ReqisterFunctions()
{
    RegisterFunction(FunctionType::Awake, std::bind(&CharacterController::Awake, this));
    RegisterFunction(FunctionType::OnStart, std::bind(&CharacterController::Init, this));
    RegisterFunction(FunctionType::PostStart, std::bind(&CharacterController::PostStart, this));
    RegisterFunction(FunctionType::Update, std::bind(&CharacterController::Update, this));
}
void CharacterController::Awake()
{
    if (auto capsuleShape = thisObject_.GetComponent<CapsuleShape>())
    {
        const auto& scale = thisObject_.GetWorldTransform().GetScale();
        shapeSize_        = capsuleShape->GetRadius() * glm::compMax(vec2(scale.x, scale.z));
    }
}
void CharacterController::Init()
{
    if (isInit)
    {
        LOG_DEBUG << "Already initialized!";
        return;
    }
    rigidbody_ = thisObject_.GetComponent<Rigidbody>();
    animator_  = thisObject_.GetComponent<Animator>();

    if (animator_ and rigidbody_)
    {
        auto aimJoint = animator_->GetJoint(aimJointName_);
        if (not aimJoint)
            LOG_ERROR << "Aim joint not found : " << aimJointName_;

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
            DodgeState(context),
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
            DisarmedFallingState(context),
            ArmedFallingState(context),
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
            animator_->jointGroups_.insert({lowerBodyGroupName, {}});
        }

        auto upperBodyGroupIter = animator_->jointGroups_.find(upperBodyGroupName);
        if (upperBodyGroupIter == animator_->jointGroups_.end())
        {
            animator_->jointGroups_.insert({upperBodyGroupName, {}});
        }
    }
    else
    {
        LOG_WARN << "Animator or rigidbody_ not exist in object";
    }

    isInit = true;
}

bool isCollision(Physics::IPhysicsApi& physicsApi, const vec3& characterPosition, const vec3& offset)
{
    const auto raySource = characterPosition + offset;
    const auto rayTarget = characterPosition + (200.f * VECTOR_DOWN) + offset;

    if (auto maybeHit = physicsApi.RayTest(raySource, rayTarget))
    {
        auto distance = glm::length(maybeHit->pointWorld - raySource);
        return distance < 2.f;
    }
    else
    {
        return false;
    }
}

void CharacterController::PostStart()
{
    LOG_DEBUG << "PostStart";
    const auto& scale  = thisObject_.GetWorldTransform().GetScale();
    auto capsuleRadius = shapeSize_ / glm::compMax(vec2(scale.x, scale.z));

    groundExitSubId = componentContext_.physicsApi_.setCollisionCallback(
        rigidbody_->GetId(),
        Physics::CollisionDetection{
            .action = Physics::CollisionDetection::Action::onExit,
            .type   = Physics::CollisionDetection::Type::repeat,
            .callback =
                [&, capsuleRadius](const auto&)
            {
                impl->fsmContext->isOnAir = true;

                if (impl->stateMachine_->isCurrentStateOfType<JumpState>())
                {
                    LOG_DEBUG << "push JumpConfirmEvent";
                    pushEventToQueue(JumpConfirmEvent{});
                    return;
                }

                const auto& characterPosition = thisObject_.GetWorldTransform().GetPosition();

                bool isAwayFromGround{true};

                auto& papi = componentContext_.physicsApi_;

                for (int y = -1; y <= 1; ++y)
                {
                    for (int x = -1; x <= 1; ++x)
                    {
                        if (isCollision(papi, characterPosition,
                                        vec3(capsuleRadius * static_cast<float>(x), capsuleRadius,
                                             capsuleRadius * static_cast<float>(y))))
                        {
                            isAwayFromGround = false;
                            break;
                        }
                    }
                }

                if (isAwayFromGround)
                {
                    pushEventToQueue(StartFallingEvent{});
                }
            },
        });

    groundEnterSubId = componentContext_.physicsApi_.setCollisionCallback(
        rigidbody_->GetId(),
        Physics::CollisionDetection{
            .action = Physics::CollisionDetection::Action::onEnter,
            .type   = Physics::CollisionDetection::Type::repeat,
            .callback =
                [&](const auto& collisionInfos)
            {
                if (not collisionInfos.empty())
                {
                    for (const auto& collisionInfo : collisionInfos)
                    {
                        if (rigidbody_->GetId() == collisionInfo.rigidbodyId1)
                        {
                            LOG_DEBUG << "GroundDetectionEvent collisionWith: " << collisionInfo.rigidbodyId2;
                            pushEventToFrontQueue(GroundDetectionEvent{});
                            impl->fsmContext->isOnAir = false;
                            break;
                        }
                        else
                        {
                            LOG_DEBUG << "GroundDetectionEvent collisionWith: " << collisionInfo.rigidbodyId1;
                            pushEventToFrontQueue(GroundDetectionEvent{});
                            impl->fsmContext->isOnAir = false;
                            break;
                        }
                    }
                }
            },
            .ignoredList = {},
            .predicate =
                [&, capsuleRadius](const auto& collisionInfo)
            {
                const auto& characterPosition     = thisObject_.GetWorldTransform().GetPosition();
                const auto characterPosWithOffset = characterPosition + vec3(0, capsuleRadius, 0);

                if (rigidbody_->GetId() == collisionInfo.rigidbodyId1)
                {
                    return (collisionInfo.pos2.y <= characterPosWithOffset.y);
                }
                else
                {
                    return (collisionInfo.pos1.y <= characterPosWithOffset.y);
                }
            }});
}
void CharacterController::processEvent()
{
    EventQueue tmpEventsQueue;

    {
        std::lock_guard<std::mutex> lk(eventQueueMutex);
        tmpEventsQueue = std::move(eventQueue);
    }

    for (auto& event : tmpEventsQueue)
    {
        handleEvent(event);
    }
}
void CharacterController::Update()
{
    processEvent();

    if (impl->stateMachine_ and rigidbody_ and rigidbody_->IsReady())
    {
        std::visit(
            [&](auto statePtr)
            {
                statePtr->update(componentContext_.time_.deltaTime);
            },
            impl->stateMachine_->currentState);
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

    regsiterComponentReadFunction(GetComponentType<CharacterController>(), readFunc);
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
