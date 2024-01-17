#include <Utils/Time/Timer.h>
#include <gtest/gtest.h>

#include "BaseComponent.h"
#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/Animation/JointPoseUpdater.h"
#include "GameEngine/Components/Controllers/CharacterController.h"
#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/Components/Physics/SphereShape.h"
#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"

using namespace GameEngine;
using namespace GameEngine::Components;
using namespace ::testing;

namespace
{
const int DUMMY_FRAMES                    = 4;
const float DUMMY_FRAME_TIME_DELTA        = 0.25f;
const float DUMMY_CLIP_LENGTH             = DUMMY_FRAME_TIME_DELTA * static_cast<float>(DUMMY_FRAMES);
const float ADVANCED_TIME_TRANSITION_TIME = DEFAULT_ANIMATION_TRANSITION_TIME + 0.1f;
const float ADVANCED_TIME_CLIP_TIME       = DUMMY_CLIP_LENGTH + 0.1f;
}  // namespace

struct CharacterControllerTests : public BaseComponentTestSchould
{
    CharacterControllerTests()
        : BaseComponentTestSchould()
        , sut_(context_, obj_)
    {
        EXPECT_CALL(physicsApiMock_, CreateSphereColider(_, _, _)).WillOnce(Return(shapeId));
        EXPECT_CALL(physicsApiMock_, CreateRigidbody(shapeId, _, _, _, _)).WillOnce(Return(rigidbodyid));

        animator_  = &obj_.AddComponent<Animator>();
        rigidbody_ = &obj_.AddComponent<Rigidbody>();

        obj_.AddComponent<RendererComponent>().AddModel(&model_);
        obj_.AddComponent<SphereShape>();

        createDummySkeleton();

        for (auto& component : obj_.GetComponents())
        {
            component->ReqisterFunctions();
        }

        sut_.ReqisterFunctions();

        componentController_.CallFunctions(FunctionType::Awake);
        componentController_.CallFunctions(FunctionType::OnStart);

        auto& clips                 = sut_.animationClipsNames_;
        clips.disarmed.idle         = "DI";
        clips.disarmed.run.forward  = "DRF";
        clips.disarmed.run.backward = "DRB";
        clips.armed.idle            = "AI";
        clips.armed.run.forward     = "ARF";
        clips.armed.run.backward    = "ARB";
        clips.equip                 = "equip";
        clips.disarm                = "disarm";

        addDummyClip(clips.equip);
        addDummyClip(clips.disarm);
        addDummyClip(clips.disarmed.idle);
        addDummyClip(clips.disarmed.run.forward);
        addDummyClip(clips.disarmed.run.backward);
        addDummyClip(clips.armed.idle);
        addDummyClip(clips.armed.run.forward);
        addDummyClip(clips.armed.run.backward);
        sut_.equipTimeStamp  = DUMMY_CLIP_LENGTH;
        sut_.disarmTimeStamp = DUMMY_CLIP_LENGTH;

        auto weaponPtr =
            std::make_unique<GameObject>("WeaponObjName", componentController_, componentFactory_, IdType(1));
        weaponPtr->AddComponent<JointPoseUpdater>();
        obj_.AddChild(std::move(weaponPtr));

        sut_.Init();
        gpuResourceLoader_.RuntimeGpuTasks();
    }
    virtual void SetUp() override
    {
        ASSERT_TRUE(sut_.fsm() != nullptr);
        EXPECT_CALL(physicsApiMock_, RemoveShape(shapeId));
        EXPECT_CALL(physicsApiMock_, RemoveRigidBody(rigidbodyid));
    }

    virtual void TearDown() override
    {
    }

    void createDummySkeleton()
    {
        animator_->jointGroups_.insert({sut_.lowerBodyGroupName, {"head", "neck", "leftHand", "rightHand"}});
        animator_->jointGroups_.insert({sut_.upperBodyGroupName, {"hips, leftLeg", "rightLeg"}});

        Animation::JointId id = 0;
        Animation::Joint rootJoint;
        rootJoint.name = "Armature";
        rootJoint.id   = id;

        for (const auto& [_, group] : animator_->jointGroups_)
        {
            for (const auto& jointName : group)
            {
                Animation::Joint joint;
                joint.name = jointName;
                joint.id   = ++id;
                rootJoint.addChild(joint);
            }
        }

        model_.setRootJoint(rootJoint);
    }

    void addDummyClip(const std::string& name)
    {
        Animation::AnimationClip clip(name);
        for (int i = 0; i <= DUMMY_FRAMES; ++i)
        {
            clip.AddFrame(Animation::KeyFrame{DUMMY_FRAME_TIME_DELTA * (float)i, {{0, Animation::JointTransform{}}}});
        }
        DEBUG_LOG("addDummyClip : " + name + " Length : " + std::to_string(clip.GetLength()));
        animator_->AddAnimationClip(clip);
    }

    void Update(float time)
    {
        DEBUG_LOG("Update");
        context_.time_.deltaTime = time;
        componentController_.CallFunctions(FunctionType::Update);
        sut_.Update();
    }

    void expectAnimsToBeSet(const std::vector<std::string>& names)
    {
        EXPECT_EQ(animator_->getCurrentAnimationName().size(), names.size());

        for(const auto& name : animator_->getCurrentAnimationName())
        {
            auto iter = std::find(names.begin(), names.end(), name);
            EXPECT_TRUE(iter != names.end());
        }
    }

    CharacterController sut_;
    Animator* animator_{nullptr};
    Rigidbody* rigidbody_{nullptr};

    GameEngine::Model model_;
    GameEngine::Physics::ShapeId shapeId{1};
    GameEngine::Physics::RigidbodyId rigidbodyid{1};
};

TEST_F(CharacterControllerTests, DisarmedState_IdleToRunForward)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, SetVelocityRigidbody(rigidbodyid, vec3(0.0, 0.0, DEFAULT_RUN_SPEED)))
        .Times(AtLeast(1));

    sut_.fsm()->handle(MoveForwardEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.run.forward});
}

TEST_F(CharacterControllerTests, DisarmedState_IdleToRunBackward)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, SetVelocityRigidbody(rigidbodyid, vec3(0.0, 0.0, -DEFAULT_BACKWARD_RUN_SPEED)))
        .Times(AtLeast(1));

    sut_.fsm()->handle(MoveBackwardEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.run.backward});
}

TEST_F(CharacterControllerTests, DisarmedState_IdleToRunForwardAndBackward)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, SetVelocityRigidbody(rigidbodyid, vec3(0.0, 0.0, DEFAULT_RUN_SPEED)))
        .Times(AtLeast(1));
    ;

    sut_.fsm()->handle(MoveForwardEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.run.forward});

    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, SetVelocityRigidbody(rigidbodyid, vec3(0.0, 0.0, -DEFAULT_BACKWARD_RUN_SPEED)))
        .Times(AtLeast(1));
    ;
    sut_.fsm()->handle(MoveBackwardEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.run.backward});

    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, SetVelocityRigidbody(rigidbodyid, vec3(0.0, 0.0, DEFAULT_RUN_SPEED)))
        .Times(AtLeast(1));
    ;
    sut_.fsm()->handle(EndBackwardMoveEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.run.forward});
}

TEST_F(CharacterControllerTests, DisarmedToArmedStateIdle)
{
    sut_.fsm()->handle(WeaponStateEvent{});

    // Animation transition : None -> Armed idle state
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.equip});

    // Animation : EquipAnim
    Update(ADVANCED_TIME_CLIP_TIME);

    // State need to change
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.idle});
}

TEST_F(CharacterControllerTests, RunForwardDuringDisarmedToArmedState)
{
    sut_.fsm()->handle(WeaponStateEvent{});

    // Animation transition : None -> Armed idle state (
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.equip});

    // Animation : EquipAnim
    Update(DUMMY_CLIP_LENGTH / 2.f);

    sut_.fsm()->handle(MoveForwardEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.run.forward});
}

TEST_F(CharacterControllerTests, EquipWeaponDuringDisarmedRunForward)
{
    vec3 runningVelocity(0.0, 0.0, DEFAULT_RUN_SPEED);

    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, SetVelocityRigidbody(rigidbodyid, runningVelocity)).Times(AtLeast(1));

    sut_.fsm()->handle(MoveForwardEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.run.forward});

    sut_.fsm()->handle(WeaponStateEvent{});

    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(runningVelocity));

    // PlayMixedAnimation with equip
    Update(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.run.forward, sut_.animationClipsNames_.equip});

    // Animation : Tirgger EquipAnim end, and notify state
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.run.forward});
    Update(ADVANCED_TIME_TRANSITION_TIME);
}
