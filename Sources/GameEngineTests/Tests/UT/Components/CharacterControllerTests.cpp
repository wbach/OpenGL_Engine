#include <Utils/Time/Timer.h>
#include <gtest/gtest.h>

#include "BaseComponent.h"
#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/Animation/JointPoseUpdater.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterController.h"
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

        auto& clips                  = sut_.animationClipsNames_;
        clips.disarmed.idle          = "DI";
        clips.disarmed.sprint        = "DS";
        clips.disarmed.run.forward   = "DRF";
        clips.disarmed.run.backward  = "DRB";
        clips.disarmed.walk.forward  = "DWF";
        clips.disarmed.walk.backward = "DWB";
        clips.armed.idle             = "AI";
        clips.armed.sprint           = "AS";
        clips.armed.run.forward      = "ARF";
        clips.armed.run.backward     = "ARB";
        clips.armed.walk.forward     = "AWF";
        clips.armed.walk.backward    = "AWB";
        clips.equip                  = "equip";
        clips.disarm                 = "disarm";
        clips.disarmed.rotateLeft    = "DRL";
        clips.disarmed.rotateRight   = "DRR";
        clips.armed.rotateLeft       = "ARL";
        clips.armed.rotateRight      = "ARR";
        clips.drawArrow              = "drawArrow";
        clips.recoilArrow            = "recoilArrow";
        clips.aimIdle                = "aimIdle";

        addDummyClip(clips.equip);
        addDummyClip(clips.disarm);
        addDummyClip(clips.disarmed.idle);
        addDummyClip(clips.disarmed.sprint);
        addDummyClip(clips.disarmed.run.forward);
        addDummyClip(clips.disarmed.run.backward);
        addDummyClip(clips.disarmed.walk.forward);
        addDummyClip(clips.disarmed.walk.backward);
        addDummyClip(clips.armed.idle);
        addDummyClip(clips.armed.sprint);
        addDummyClip(clips.armed.run.forward);
        addDummyClip(clips.armed.run.backward);
        addDummyClip(clips.armed.walk.forward);
        addDummyClip(clips.armed.walk.backward);
        addDummyClip(clips.disarmed.rotateLeft);
        addDummyClip(clips.disarmed.rotateRight);
        addDummyClip(clips.armed.rotateLeft);
        addDummyClip(clips.armed.rotateRight);
        addDummyClip(clips.drawArrow);
        addDummyClip(clips.recoilArrow);
        addDummyClip(clips.aimIdle);

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

        Update(ADVANCED_TIME_TRANSITION_TIME);  // To set first anim
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

        for (const auto& name : names)
        {
            DEBUG_LOG("Expected : " + name);
        }

        for (const auto& name : animator_->getCurrentAnimationName())
        {
            DEBUG_LOG("Current  : " + name);
            auto iter = std::find(names.begin(), names.end(), name);
            EXPECT_TRUE(iter != names.end());
        }
    }

    template <typename State>
    void expectState()
    {
        EXPECT_TRUE(std::holds_alternative<State*>(sut_.fsm()->getCurrentState()));
    }

    template <typename Event, typename State>
    void tiggerAndExpect(const std::vector<std::string>& clipNames,
                         std::vector<float> updateTimes = {ADVANCED_TIME_TRANSITION_TIME})
    {
        sut_.fsm()->handle(Event{});
        for (auto updateTime : updateTimes)
            Update(updateTime);
        expectState<State>();
        expectAnimsToBeSet(clipNames);
    }

    void expectForwardVelocity(float speed)
    {
        EXPECT_CALL(physicsApiMock_, SetVelocityRigidbody(rigidbodyid, vec3(0.0, 0.0, speed))).Times(AtLeast(1));
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

    sut_.fsm()->handle(RunForwardEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.run.forward});

    sut_.fsm()->handle(EndForwardMoveEvent{});
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
}

TEST_F(CharacterControllerTests, DisarmedState_IdleToRunBackward)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, SetVelocityRigidbody(rigidbodyid, vec3(0.0, 0.0, -DEFAULT_BACKWARD_RUN_SPEED)))
        .Times(AtLeast(1));

    sut_.fsm()->handle(RunBackwardEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.run.backward});

    sut_.fsm()->handle(EndBackwardMoveEvent{});
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
}

TEST_F(CharacterControllerTests, DisarmedState_IdleToRunForwardAndBackward)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, SetVelocityRigidbody(rigidbodyid, vec3(0.0, 0.0, DEFAULT_RUN_SPEED)))
        .Times(AtLeast(1));

    sut_.fsm()->handle(RunForwardEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.run.forward});

    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, SetVelocityRigidbody(rigidbodyid, vec3(0.0, 0.0, -DEFAULT_BACKWARD_RUN_SPEED)))
        .Times(AtLeast(1));
    ;
    sut_.fsm()->handle(RunBackwardEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.run.backward});

    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, SetVelocityRigidbody(rigidbodyid, vec3(0.0, 0.0, DEFAULT_RUN_SPEED)))
        .Times(AtLeast(1));

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

    sut_.fsm()->handle(RunForwardEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.run.forward, sut_.animationClipsNames_.equip});

    Update(DUMMY_CLIP_LENGTH / 2.f);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.run.forward});
}

TEST_F(CharacterControllerTests, EquipWeaponDuringDisarmedRunForward)
{
    vec3 runningVelocity(0.0, 0.0, DEFAULT_RUN_SPEED);

    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, SetVelocityRigidbody(rigidbodyid, runningVelocity)).Times(AtLeast(1));

    sut_.fsm()->handle(RunForwardEvent{});

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

TEST_F(CharacterControllerTests, DisarmWeaponDuringArmedRunForward)
{
    vec3 runningVelocity(0.0, 0.0, DEFAULT_RUN_SPEED);

    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, SetVelocityRigidbody(rigidbodyid, runningVelocity)).Times(AtLeast(1));

    sut_.fsm()->handle(WeaponStateEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);

    sut_.fsm()->handle(RunForwardEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.run.forward});

    sut_.fsm()->handle(WeaponStateEvent{});

    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(runningVelocity));

    // PlayMixedAnimation with equip
    Update(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.run.forward, sut_.animationClipsNames_.disarm});

    // Animation : Tirgger EquipAnim end, and notify state
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.run.forward});
    Update(ADVANCED_TIME_TRANSITION_TIME);
}

TEST_F(CharacterControllerTests, EquipWeaponDuringRotateLeftState)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    sut_.fsm()->handle(RotateLeftEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.rotateLeft});

    sut_.fsm()->handle(WeaponStateEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.rotateLeft, sut_.animationClipsNames_.equip});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.rotateLeft});

    sut_.fsm()->handle(WeaponStateEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.rotateLeft, sut_.animationClipsNames_.disarm});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.rotateLeft});

    sut_.fsm()->handle(EndRotationEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
}

TEST_F(CharacterControllerTests, EquipWeaponDuringRotateRightState)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    sut_.fsm()->handle(RotateRightEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.rotateRight});

    sut_.fsm()->handle(WeaponStateEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.rotateRight, sut_.animationClipsNames_.equip});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.rotateRight});

    sut_.fsm()->handle(WeaponStateEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.rotateRight, sut_.animationClipsNames_.disarm});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.rotateRight});

    sut_.fsm()->handle(EndRotationEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
}

TEST_F(CharacterControllerTests, DisarmedStateToArmed_RunForwardAndRotateLeft)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, SetVelocityRigidbody(rigidbodyid, vec3(0.0, 0.0, DEFAULT_RUN_SPEED)))
        .Times(AtLeast(1));

    sut_.fsm()->handle(RunForwardEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.run.forward});

    sut_.fsm()->handle(RotateLeftEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.run.forward});

    sut_.fsm()->handle(WeaponStateEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.run.forward, sut_.animationClipsNames_.equip});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.run.forward});

    sut_.fsm()->handle(EndRotationEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.run.forward});

    sut_.fsm()->handle(EndForwardMoveEvent{});
    ;

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.idle});
}

TEST_F(CharacterControllerTests, DisarmedStateToArmed_RunForwardAndRotateRight)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, SetVelocityRigidbody(rigidbodyid, vec3(0.0, 0.0, DEFAULT_RUN_SPEED)))
        .Times(AtLeast(1));

    sut_.fsm()->handle(RunForwardEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.run.forward});

    sut_.fsm()->handle(RotateRightEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.run.forward});

    sut_.fsm()->handle(WeaponStateEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.run.forward, sut_.animationClipsNames_.equip});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.run.forward});

    sut_.fsm()->handle(EndRotationEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.run.forward});

    sut_.fsm()->handle(EndForwardMoveEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.idle});
}

TEST_F(CharacterControllerTests, ArmedState_RunForwardAndRotateLeftAndChangeWeaponState)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, SetVelocityRigidbody(rigidbodyid, vec3(0.0, 0.0, DEFAULT_RUN_SPEED)))
        .Times(AtLeast(1));

    sut_.fsm()->handle(WeaponStateEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.equip});

    sut_.fsm()->handle(RunForwardEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.run.forward, sut_.animationClipsNames_.equip});

    sut_.fsm()->handle(RotateLeftEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.run.forward, sut_.animationClipsNames_.equip});

    sut_.fsm()->handle(WeaponStateEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.run.forward, sut_.animationClipsNames_.disarm});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.run.forward});

    sut_.fsm()->handle(EndForwardMoveEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.rotateLeft});

    sut_.fsm()->handle(EndRotationEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
}
TEST_F(CharacterControllerTests, DisamredToArmedState_DuringEquipRunForwardAndRotateLeftAndChangeWeaponState)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, SetVelocityRigidbody(rigidbodyid, vec3(0.0, 0.0, DEFAULT_RUN_SPEED)))
        .Times(AtLeast(1));

    sut_.fsm()->handle(WeaponStateEvent{});
    sut_.fsm()->handle(RunForwardEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.run.forward, sut_.animationClipsNames_.equip});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.run.forward});
}

TEST_F(CharacterControllerTests, DisarmedStopMovingDuringEuip)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, SetVelocityRigidbody(rigidbodyid, vec3(0.0, 0.0, DEFAULT_RUN_SPEED)))
        .Times(AtLeast(1));

    sut_.fsm()->handle(RunForwardEvent{});
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.run.forward});
    sut_.fsm()->handle(WeaponStateEvent{});
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.run.forward, sut_.animationClipsNames_.equip});
    Update(ADVANCED_TIME_TRANSITION_TIME);
    sut_.fsm()->handle(EndForwardMoveEvent{});
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.idle});
    sut_.fsm()->handle(WeaponStateEvent{});
    Update(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
}

TEST_F(CharacterControllerTests, DisamredDuringEquip)
{
    sut_.fsm()->handle(WeaponStateEvent{});
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.equip});
    sut_.fsm()->handle(WeaponStateEvent{});
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarm});
    Update(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
}

TEST_F(CharacterControllerTests, DisamredDuringEquipTransition)
{
    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    sut_.fsm()->handle(WeaponStateEvent{});

    tiggerAndExpect<WeaponStateEvent, DisarmedIdleState>({sut_.animationClipsNames_.disarm});
    Update(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
}

TEST_F(CharacterControllerTests, IdleToWalkToIdleState)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkForwardEvent, DisarmedWalkState>({sut_.animationClipsNames_.disarmed.walk.forward});
    tiggerAndExpect<EndForwardMoveEvent, DisarmedIdleState>({sut_.animationClipsNames_.disarmed.idle});
}

TEST_F(CharacterControllerTests, IdleChangeWalkEventDoNothing)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<WalkChangeStateEvent, DisarmedIdleState>({sut_.animationClipsNames_.disarmed.idle});
}

TEST_F(CharacterControllerTests, RunToWalkToRunState)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<RunForwardEvent, DisarmedRunState>({sut_.animationClipsNames_.disarmed.run.forward});
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent, DisarmedWalkState>({sut_.animationClipsNames_.disarmed.walk.forward});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<WalkChangeStateEvent, DisarmedRunState>({sut_.animationClipsNames_.disarmed.run.forward});
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent, DisarmedWalkState>({sut_.animationClipsNames_.disarmed.walk.forward});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<RunForwardEvent, DisarmedRunState>({sut_.animationClipsNames_.disarmed.run.forward});
}

TEST_F(CharacterControllerTests, WalkAndRotateState)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, SetVelocityRigidbody(rigidbodyid, vec3(0.0, 0.0, DEFAULT_WALK_SPEED)))
        .Times(AtLeast(1));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});

    tiggerAndExpect<WalkForwardEvent, DisarmedWalkState>({sut_.animationClipsNames_.disarmed.walk.forward});
    tiggerAndExpect<RotateRightEvent, DisarmedWalkAndRotateState>({sut_.animationClipsNames_.disarmed.walk.forward});
    tiggerAndExpect<EndForwardMoveEvent, DisarmedRotateState>({sut_.animationClipsNames_.disarmed.rotateRight});
}

TEST_F(CharacterControllerTests, RotateAndWalkState)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, SetVelocityRigidbody(rigidbodyid, vec3(0.0, 0.0, DEFAULT_WALK_SPEED)))
        .Times(AtLeast(1));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});

    tiggerAndExpect<RotateRightEvent, DisarmedRotateState>({sut_.animationClipsNames_.disarmed.rotateRight});
    tiggerAndExpect<WalkForwardEvent, DisarmedWalkAndRotateState>({sut_.animationClipsNames_.disarmed.walk.forward});
    tiggerAndExpect<EndForwardMoveEvent, DisarmedRotateState>({sut_.animationClipsNames_.disarmed.rotateRight});
}

TEST_F(CharacterControllerTests, RunAndRotateToWalkAndRotate)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});

    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<RunForwardEvent, DisarmedRunState>({sut_.animationClipsNames_.disarmed.run.forward});
    tiggerAndExpect<RotateRightEvent, DisarmedRunAndRotateState>({sut_.animationClipsNames_.disarmed.run.forward});
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent, DisarmedWalkAndRotateState>(
        {sut_.animationClipsNames_.disarmed.walk.forward});
    tiggerAndExpect<EndForwardMoveEvent, DisarmedRotateState>({sut_.animationClipsNames_.disarmed.rotateRight});
}

TEST_F(CharacterControllerTests, RunAndRotateToWalkAndRotateToWalkForward)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(0));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});

    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<RunForwardEvent, DisarmedRunState>({sut_.animationClipsNames_.disarmed.run.forward});
    tiggerAndExpect<RotateRightEvent, DisarmedRunAndRotateState>({sut_.animationClipsNames_.disarmed.run.forward});

    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent, DisarmedWalkAndRotateState>(
        {sut_.animationClipsNames_.disarmed.walk.forward});
    tiggerAndExpect<EndRotationEvent, DisarmedWalkState>({sut_.animationClipsNames_.disarmed.walk.forward});
}

TEST_F(CharacterControllerTests, DisarmedIdleToWalkForwardBackward)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});

    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkForwardEvent, DisarmedWalkState>({sut_.animationClipsNames_.disarmed.walk.forward});
    expectForwardVelocity(-DEFAULT_BACKWARD_WALK_SPEED);
    tiggerAndExpect<WalkBackwardEvent, DisarmedWalkState>({sut_.animationClipsNames_.disarmed.walk.backward});
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<EndBackwardMoveEvent, DisarmedWalkState>({sut_.animationClipsNames_.disarmed.walk.forward});
    tiggerAndExpect<EndForwardMoveEvent, DisarmedIdleState>({sut_.animationClipsNames_.disarmed.idle});
}

TEST_F(CharacterControllerTests, DisarmedIdleToWalkBackwardForward)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});

    expectForwardVelocity(-DEFAULT_BACKWARD_WALK_SPEED);
    tiggerAndExpect<WalkBackwardEvent, DisarmedWalkState>({sut_.animationClipsNames_.disarmed.walk.backward});
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkForwardEvent, DisarmedWalkState>({sut_.animationClipsNames_.disarmed.walk.forward});
    expectForwardVelocity(-DEFAULT_BACKWARD_WALK_SPEED);
    tiggerAndExpect<EndForwardMoveEvent, DisarmedWalkState>({sut_.animationClipsNames_.disarmed.walk.backward});
    tiggerAndExpect<EndBackwardMoveEvent, DisarmedIdleState>({sut_.animationClipsNames_.disarmed.idle});
}

TEST_F(CharacterControllerTests, ArmedIdleToWalkForward)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkForwardEvent, ArmedWalkState>({sut_.animationClipsNames_.armed.walk.forward});
    tiggerAndExpect<EndForwardMoveEvent, ArmedIdleState>({sut_.animationClipsNames_.armed.idle});
}

TEST_F(CharacterControllerTests, ArmedIdleToWalkBackward)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
    expectForwardVelocity(-DEFAULT_BACKWARD_WALK_SPEED);
    tiggerAndExpect<WalkBackwardEvent, ArmedWalkState>({sut_.animationClipsNames_.armed.walk.backward});
    tiggerAndExpect<EndBackwardMoveEvent, ArmedIdleState>({sut_.animationClipsNames_.armed.idle});
}

TEST_F(CharacterControllerTests, ArmedIdleToWalkForwardBackward)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkForwardEvent, ArmedWalkState>({sut_.animationClipsNames_.armed.walk.forward});
    expectForwardVelocity(-DEFAULT_BACKWARD_WALK_SPEED);
    tiggerAndExpect<WalkBackwardEvent, ArmedWalkState>({sut_.animationClipsNames_.armed.walk.backward});
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<EndBackwardMoveEvent, ArmedWalkState>({sut_.animationClipsNames_.armed.walk.forward});
    tiggerAndExpect<EndForwardMoveEvent, ArmedIdleState>({sut_.animationClipsNames_.armed.idle});
}

TEST_F(CharacterControllerTests, ArmedIdleDoNothingIfWalkChangeEvent)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
    tiggerAndExpect<WalkChangeStateEvent, ArmedIdleState>({sut_.animationClipsNames_.armed.idle});
}

TEST_F(CharacterControllerTests, ArmedRunToWalkForward)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkForwardEvent, ArmedWalkState>({sut_.animationClipsNames_.armed.walk.forward});
    expectForwardVelocity(-DEFAULT_BACKWARD_WALK_SPEED);
    tiggerAndExpect<WalkBackwardEvent, ArmedWalkState>({sut_.animationClipsNames_.armed.walk.backward});
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<EndBackwardMoveEvent, ArmedWalkState>({sut_.animationClipsNames_.armed.walk.forward});
    tiggerAndExpect<EndForwardMoveEvent, ArmedIdleState>({sut_.animationClipsNames_.armed.idle});
}

TEST_F(CharacterControllerTests, ArmedRunToWalkForwardToRun)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkForwardEvent, ArmedWalkState>({sut_.animationClipsNames_.armed.walk.forward});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<WalkChangeStateEvent, ArmedRunState>({sut_.animationClipsNames_.armed.run.forward});
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent, ArmedWalkState>({sut_.animationClipsNames_.armed.walk.forward});
    tiggerAndExpect<EndForwardMoveEvent, ArmedIdleState>({sut_.animationClipsNames_.armed.idle});
}

TEST_F(CharacterControllerTests, ArmedRunToWalkBackwardToRun)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
    expectForwardVelocity(-DEFAULT_BACKWARD_WALK_SPEED);
    tiggerAndExpect<WalkBackwardEvent, ArmedWalkState>({sut_.animationClipsNames_.armed.walk.backward});
    expectForwardVelocity(-DEFAULT_BACKWARD_RUN_SPEED);
    tiggerAndExpect<WalkChangeStateEvent, ArmedRunState>({sut_.animationClipsNames_.armed.run.backward});
    expectForwardVelocity(-DEFAULT_BACKWARD_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent, ArmedWalkState>({sut_.animationClipsNames_.armed.walk.backward});
    tiggerAndExpect<EndBackwardMoveEvent, ArmedIdleState>({sut_.animationClipsNames_.armed.idle});
}

TEST_F(CharacterControllerTests, ArmedRunAndRotateForwardToWalkAndRotateForwardAndRotation)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(0));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    tiggerAndExpect<RotateLeftEvent, ArmedRotateState>({sut_.animationClipsNames_.armed.rotateLeft});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<RunForwardEvent, ArmedRunAndRotateState>({sut_.animationClipsNames_.armed.run.forward});

    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent, ArmedWalkAndRotateState>({sut_.animationClipsNames_.armed.walk.forward});
    tiggerAndExpect<RotateRightEvent, ArmedWalkAndRotateState>({sut_.animationClipsNames_.armed.walk.forward});

    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<WalkChangeStateEvent, ArmedRunAndRotateState>({sut_.animationClipsNames_.armed.run.forward});
    tiggerAndExpect<RotateLeftEvent, ArmedRunAndRotateState>({sut_.animationClipsNames_.armed.run.forward});

    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent, ArmedWalkAndRotateState>({sut_.animationClipsNames_.armed.walk.forward});
    tiggerAndExpect<RotateRightEvent, ArmedWalkAndRotateState>({sut_.animationClipsNames_.armed.walk.forward});

    tiggerAndExpect<EndRotationEvent, ArmedWalkState>({sut_.animationClipsNames_.armed.walk.forward});
    tiggerAndExpect<EndForwardMoveEvent, ArmedIdleState>({sut_.animationClipsNames_.armed.idle});
}

TEST_F(CharacterControllerTests, ArmedRunAndRotateForwardToWalkAndRotateBackwardAndRotation)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(0));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    tiggerAndExpect<RotateLeftEvent, ArmedRotateState>({sut_.animationClipsNames_.armed.rotateLeft});
    expectForwardVelocity(-DEFAULT_BACKWARD_RUN_SPEED);
    tiggerAndExpect<RunBackwardEvent, ArmedRunAndRotateState>({sut_.animationClipsNames_.armed.run.backward});

    expectForwardVelocity(-DEFAULT_BACKWARD_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent, ArmedWalkAndRotateState>({sut_.animationClipsNames_.armed.walk.backward});
    tiggerAndExpect<RotateRightEvent, ArmedWalkAndRotateState>({sut_.animationClipsNames_.armed.walk.backward});

    expectForwardVelocity(-DEFAULT_BACKWARD_RUN_SPEED);
    tiggerAndExpect<WalkChangeStateEvent, ArmedRunAndRotateState>({sut_.animationClipsNames_.armed.run.backward});
    tiggerAndExpect<RotateLeftEvent, ArmedRunAndRotateState>({sut_.animationClipsNames_.armed.run.backward});

    expectForwardVelocity(-DEFAULT_BACKWARD_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent, ArmedWalkAndRotateState>({sut_.animationClipsNames_.armed.walk.backward});
    tiggerAndExpect<RotateRightEvent, ArmedWalkAndRotateState>({sut_.animationClipsNames_.armed.walk.backward});

    tiggerAndExpect<EndRotationEvent, ArmedWalkState>({sut_.animationClipsNames_.armed.walk.backward});
    tiggerAndExpect<EndBackwardMoveEvent, ArmedIdleState>({sut_.animationClipsNames_.armed.idle});
}

TEST_F(CharacterControllerTests, IdleToWalkChangeDuringEquip)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<WeaponStateEvent, ArmedIdleState>({sut_.animationClipsNames_.equip},
                                                      {ADVANCED_TIME_TRANSITION_TIME});

    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkForwardEvent, ArmedWalkState>(
        {sut_.animationClipsNames_.armed.walk.forward, sut_.animationClipsNames_.equip});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectState<ArmedWalkState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.walk.forward});
}

TEST_F(CharacterControllerTests, RunToWalkChangeDuringEquip)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<RunForwardEvent, DisarmedRunState>({sut_.animationClipsNames_.disarmed.run.forward});
    tiggerAndExpect<WeaponStateEvent, ArmedRunState>(
        {sut_.animationClipsNames_.armed.run.forward, sut_.animationClipsNames_.equip},
        {ADVANCED_TIME_TRANSITION_TIME});

    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkForwardEvent, ArmedWalkState>(
        {sut_.animationClipsNames_.armed.walk.forward, sut_.animationClipsNames_.equip});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectState<ArmedWalkState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.walk.forward});
}

TEST_F(CharacterControllerTests, ArmedRunToWalkChangeDuringDisarm)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<RunForwardEvent, DisarmedRunState>({sut_.animationClipsNames_.disarmed.run.forward});
    tiggerAndExpect<WeaponStateEvent, ArmedRunState>(
        {sut_.animationClipsNames_.armed.run.forward},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
    tiggerAndExpect<WeaponStateEvent, DisarmedRunState>(
        {sut_.animationClipsNames_.disarmed.run.forward, sut_.animationClipsNames_.disarm},
        {ADVANCED_TIME_TRANSITION_TIME});
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent, DisarmedWalkState>(
        {sut_.animationClipsNames_.disarmed.walk.forward, sut_.animationClipsNames_.disarm});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectState<DisarmedWalkState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.walk.forward});
}

TEST_F(CharacterControllerTests, ArmedWalkToRunChangeDuringDisarm)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkForwardEvent, DisarmedWalkState>({sut_.animationClipsNames_.disarmed.walk.forward});
    tiggerAndExpect<WeaponStateEvent, ArmedWalkState>(
        {sut_.animationClipsNames_.armed.walk.forward},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
    tiggerAndExpect<WeaponStateEvent, DisarmedWalkState>(
        {sut_.animationClipsNames_.disarmed.walk.forward, sut_.animationClipsNames_.disarm});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<WalkChangeStateEvent, DisarmedRunState>(
        {sut_.animationClipsNames_.disarmed.run.forward, sut_.animationClipsNames_.disarm});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.run.forward});
    tiggerAndExpect<EndForwardMoveEvent, DisarmedIdleState>({sut_.animationClipsNames_.disarmed.idle});
}

TEST_F(CharacterControllerTests, DisarmeRunBackwardIgnoreSprintEvent)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});

    expectForwardVelocity(-DEFAULT_BACKWARD_RUN_SPEED);
    tiggerAndExpect<RunBackwardEvent, DisarmedRunState>({sut_.animationClipsNames_.disarmed.run.backward});
    tiggerAndExpect<SprintStartEvent, DisarmedRunState>({sut_.animationClipsNames_.disarmed.run.backward});
    tiggerAndExpect<EndBackwardMoveEvent, DisarmedIdleState>({sut_.animationClipsNames_.disarmed.idle});
}

TEST_F(CharacterControllerTests, DisarmeRunAndRotateBackwardIgnoreSprintEvent)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(0));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});

    expectForwardVelocity(-DEFAULT_BACKWARD_RUN_SPEED);
    tiggerAndExpect<RotateLeftEvent, DisarmedRotateState>({sut_.animationClipsNames_.disarmed.rotateLeft});
    tiggerAndExpect<RunBackwardEvent, DisarmedRunAndRotateState>({sut_.animationClipsNames_.disarmed.run.backward});
    tiggerAndExpect<SprintStateChangeEvent, DisarmedRunAndRotateState>(
        {sut_.animationClipsNames_.disarmed.run.backward});
    tiggerAndExpect<EndRotationEvent, DisarmedRunState>({sut_.animationClipsNames_.disarmed.run.backward});
    tiggerAndExpect<EndBackwardMoveEvent, DisarmedIdleState>({sut_.animationClipsNames_.disarmed.idle});
}

TEST_F(CharacterControllerTests, DisarmedWalkAndRotateBackwardIgnoreSprintEvent)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(0));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});

    expectForwardVelocity(-DEFAULT_BACKWARD_WALK_SPEED);
    tiggerAndExpect<RotateLeftEvent, DisarmedRotateState>({sut_.animationClipsNames_.disarmed.rotateLeft});
    tiggerAndExpect<WalkBackwardEvent, DisarmedWalkAndRotateState>({sut_.animationClipsNames_.disarmed.walk.backward});
    tiggerAndExpect<SprintStateChangeEvent, DisarmedWalkAndRotateState>(
        {sut_.animationClipsNames_.disarmed.walk.backward});
    tiggerAndExpect<EndRotationEvent, DisarmedWalkState>({sut_.animationClipsNames_.disarmed.walk.backward});
    tiggerAndExpect<EndBackwardMoveEvent, DisarmedIdleState>({sut_.animationClipsNames_.disarmed.idle});
}

TEST_F(CharacterControllerTests, DisarmeWalkBackwardIgnoreSprintEvent)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});

    expectForwardVelocity(-DEFAULT_BACKWARD_WALK_SPEED);
    tiggerAndExpect<WalkBackwardEvent, DisarmedWalkState>({sut_.animationClipsNames_.disarmed.walk.backward});
    tiggerAndExpect<SprintStartEvent, DisarmedWalkState>({sut_.animationClipsNames_.disarmed.walk.backward});
    tiggerAndExpect<EndBackwardMoveEvent, DisarmedIdleState>({sut_.animationClipsNames_.disarmed.idle});
}

TEST_F(CharacterControllerTests, DisarmeIdleToSprint)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStartEvent, DisarmedSprintState>({sut_.animationClipsNames_.disarmed.sprint});
    tiggerAndExpect<EndForwardMoveEvent, DisarmedIdleState>({sut_.animationClipsNames_.disarmed.idle});
}

TEST_F(CharacterControllerTests, DisarmeRotateToRotateAndSprint)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(0));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<RotateLeftEvent, DisarmedRotateState>({sut_.animationClipsNames_.disarmed.rotateLeft});

    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStartEvent, DisarmedSprintAndRotateState>({sut_.animationClipsNames_.disarmed.sprint});
    tiggerAndExpect<EndForwardMoveEvent, DisarmedRotateState>({sut_.animationClipsNames_.disarmed.rotateLeft});
    tiggerAndExpect<EndRotationEvent, DisarmedIdleState>({sut_.animationClipsNames_.disarmed.idle});
}

TEST_F(CharacterControllerTests, DisarmeWalkToSprint)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkForwardEvent, DisarmedWalkState>({sut_.animationClipsNames_.disarmed.walk.forward});
    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent, DisarmedSprintState>({sut_.animationClipsNames_.disarmed.sprint});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<SprintStateChangeEvent, DisarmedRunState>({sut_.animationClipsNames_.disarmed.run.forward});
    tiggerAndExpect<EndForwardMoveEvent, DisarmedIdleState>({sut_.animationClipsNames_.disarmed.idle});
}

TEST_F(CharacterControllerTests, DisarmeRunToSprint)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<RunForwardEvent, DisarmedRunState>({sut_.animationClipsNames_.disarmed.run.forward});

    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent, DisarmedSprintState>({sut_.animationClipsNames_.disarmed.sprint});

    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<SprintStateChangeEvent, DisarmedRunState>({sut_.animationClipsNames_.disarmed.run.forward});

    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent, DisarmedSprintState>({sut_.animationClipsNames_.disarmed.sprint});

    tiggerAndExpect<EndForwardMoveEvent, DisarmedIdleState>({sut_.animationClipsNames_.disarmed.idle});
}

TEST_F(CharacterControllerTests, DisarmeWalkAndRotateToSprint)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(0));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkForwardEvent, DisarmedWalkState>({sut_.animationClipsNames_.disarmed.walk.forward});
    tiggerAndExpect<RotateRightEvent, DisarmedWalkAndRotateState>({sut_.animationClipsNames_.disarmed.walk.forward});
    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent, DisarmedSprintAndRotateState>({sut_.animationClipsNames_.disarmed.sprint});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<SprintStateChangeEvent, DisarmedRunAndRotateState>(
        {sut_.animationClipsNames_.disarmed.run.forward});
    tiggerAndExpect<EndForwardMoveEvent, DisarmedRotateState>({sut_.animationClipsNames_.disarmed.rotateRight});
    tiggerAndExpect<EndRotationEvent, DisarmedIdleState>({sut_.animationClipsNames_.disarmed.idle});
}

TEST_F(CharacterControllerTests, DisarmeRunAndRotateToSprint)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(0));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<RunForwardEvent, DisarmedRunState>({sut_.animationClipsNames_.disarmed.run.forward});
    tiggerAndExpect<RotateRightEvent, DisarmedRunAndRotateState>({sut_.animationClipsNames_.disarmed.run.forward});

    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent, DisarmedSprintAndRotateState>({sut_.animationClipsNames_.disarmed.sprint});

    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<SprintStateChangeEvent, DisarmedRunAndRotateState>(
        {sut_.animationClipsNames_.disarmed.run.forward});

    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent, DisarmedSprintAndRotateState>({sut_.animationClipsNames_.disarmed.sprint});
    tiggerAndExpect<EndRotationEvent, DisarmedSprintState>({sut_.animationClipsNames_.disarmed.sprint});
    tiggerAndExpect<EndForwardMoveEvent, DisarmedIdleState>({sut_.animationClipsNames_.disarmed.idle});
}

TEST_F(CharacterControllerTests, ArmedIdleToSprint)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStartEvent, ArmedSprintState>({sut_.animationClipsNames_.armed.sprint});
    tiggerAndExpect<EndForwardMoveEvent, ArmedIdleState>({sut_.animationClipsNames_.armed.idle});
}

TEST_F(CharacterControllerTests, ArmedRotateToRotateAndSprint)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(0));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    tiggerAndExpect<RotateLeftEvent, ArmedRotateState>({sut_.animationClipsNames_.armed.rotateLeft});
    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStartEvent, ArmedSprintAndRotateState>({sut_.animationClipsNames_.armed.sprint});
    tiggerAndExpect<EndForwardMoveEvent, ArmedRotateState>({sut_.animationClipsNames_.armed.rotateLeft});
    tiggerAndExpect<EndRotationEvent, ArmedIdleState>({sut_.animationClipsNames_.armed.idle});
}

TEST_F(CharacterControllerTests, ArmedWalkToSprint)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkForwardEvent, ArmedWalkState>({sut_.animationClipsNames_.armed.walk.forward});
    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent, ArmedSprintState>({sut_.animationClipsNames_.armed.sprint});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<SprintStateChangeEvent, ArmedRunState>({sut_.animationClipsNames_.armed.run.forward});
    tiggerAndExpect<EndForwardMoveEvent, ArmedIdleState>({sut_.animationClipsNames_.armed.idle});
}

TEST_F(CharacterControllerTests, ArmeRunToSprint)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<RunForwardEvent, ArmedRunState>({sut_.animationClipsNames_.armed.run.forward});

    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent, ArmedSprintState>({sut_.animationClipsNames_.armed.sprint});

    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<SprintStateChangeEvent, ArmedRunState>({sut_.animationClipsNames_.armed.run.forward});

    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent, ArmedSprintState>({sut_.animationClipsNames_.armed.sprint});

    tiggerAndExpect<EndForwardMoveEvent, ArmedIdleState>({sut_.animationClipsNames_.armed.idle});
}

TEST_F(CharacterControllerTests, ArmeWalkAndRotateToSprint)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(0));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkForwardEvent, ArmedWalkState>({sut_.animationClipsNames_.armed.walk.forward});
    tiggerAndExpect<RotateRightEvent, ArmedWalkAndRotateState>({sut_.animationClipsNames_.armed.walk.forward});
    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent, ArmedSprintAndRotateState>({sut_.animationClipsNames_.armed.sprint});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<SprintStateChangeEvent, ArmedRunAndRotateState>({sut_.animationClipsNames_.armed.run.forward});
    tiggerAndExpect<EndForwardMoveEvent, ArmedRotateState>({sut_.animationClipsNames_.armed.rotateRight});
    tiggerAndExpect<EndRotationEvent, ArmedIdleState>({sut_.animationClipsNames_.armed.idle});
}

TEST_F(CharacterControllerTests, ArmeRunAndRotateToSprint)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(0));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<RunForwardEvent, ArmedRunState>({sut_.animationClipsNames_.armed.run.forward});
    tiggerAndExpect<RotateRightEvent, ArmedRunAndRotateState>({sut_.animationClipsNames_.armed.run.forward});

    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent, ArmedSprintAndRotateState>({sut_.animationClipsNames_.armed.sprint});

    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<SprintStateChangeEvent, ArmedRunAndRotateState>({sut_.animationClipsNames_.armed.run.forward});

    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent, ArmedSprintAndRotateState>({sut_.animationClipsNames_.armed.sprint});
    tiggerAndExpect<EndRotationEvent, ArmedSprintState>({sut_.animationClipsNames_.armed.sprint});
    tiggerAndExpect<EndForwardMoveEvent, ArmedIdleState>({sut_.animationClipsNames_.armed.idle});
}

// TO do

TEST_F(CharacterControllerTests, ArmeRunBackwardIgnoreSprintEvent)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    expectForwardVelocity(-DEFAULT_BACKWARD_RUN_SPEED);
    tiggerAndExpect<RunBackwardEvent, ArmedRunState>({sut_.animationClipsNames_.armed.run.backward});
    tiggerAndExpect<SprintStartEvent, ArmedRunState>({sut_.animationClipsNames_.armed.run.backward});
    tiggerAndExpect<EndBackwardMoveEvent, ArmedIdleState>({sut_.animationClipsNames_.armed.idle});
}

TEST_F(CharacterControllerTests, ArmeRunAndRotateBackwardIgnoreSprintEvent)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(0));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    expectForwardVelocity(-DEFAULT_BACKWARD_RUN_SPEED);
    tiggerAndExpect<RotateLeftEvent, ArmedRotateState>({sut_.animationClipsNames_.armed.rotateLeft});
    tiggerAndExpect<RunBackwardEvent, ArmedRunAndRotateState>({sut_.animationClipsNames_.armed.run.backward});
    tiggerAndExpect<SprintStateChangeEvent, ArmedRunAndRotateState>({sut_.animationClipsNames_.armed.run.backward});
    tiggerAndExpect<EndRotationEvent, ArmedRunState>({sut_.animationClipsNames_.armed.run.backward});
    tiggerAndExpect<EndBackwardMoveEvent, ArmedIdleState>({sut_.animationClipsNames_.armed.idle});
}

TEST_F(CharacterControllerTests, ArmedWalkAndRotateBackwardIgnoreSprintEvent)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(0));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    expectForwardVelocity(-DEFAULT_BACKWARD_WALK_SPEED);
    tiggerAndExpect<RotateLeftEvent, ArmedRotateState>({sut_.animationClipsNames_.armed.rotateLeft});
    tiggerAndExpect<WalkBackwardEvent, ArmedWalkAndRotateState>({sut_.animationClipsNames_.armed.walk.backward});
    tiggerAndExpect<SprintStateChangeEvent, ArmedWalkAndRotateState>({sut_.animationClipsNames_.armed.walk.backward});
    tiggerAndExpect<EndRotationEvent, ArmedWalkState>({sut_.animationClipsNames_.armed.walk.backward});
    tiggerAndExpect<EndBackwardMoveEvent, ArmedIdleState>({sut_.animationClipsNames_.armed.idle});
}

TEST_F(CharacterControllerTests, ArmeWalkBackwardIgnoreSprintEvent)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    expectForwardVelocity(-DEFAULT_BACKWARD_WALK_SPEED);
    tiggerAndExpect<WalkBackwardEvent, ArmedWalkState>({sut_.animationClipsNames_.armed.walk.backward});
    tiggerAndExpect<SprintStartEvent, ArmedWalkState>({sut_.animationClipsNames_.armed.walk.backward});
    tiggerAndExpect<EndBackwardMoveEvent, ArmedIdleState>({sut_.animationClipsNames_.armed.idle});
}

TEST_F(CharacterControllerTests, DisarmedWalkAndEquipWeapon)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<WalkForwardEvent, DisarmedWalkState>({sut_.animationClipsNames_.disarmed.walk.forward});

    tiggerAndExpect<WeaponStateEvent, ArmedWalkState>(
        {sut_.animationClipsNames_.armed.walk.forward},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    tiggerAndExpect<WeaponStateEvent, DisarmedWalkState>(
        {sut_.animationClipsNames_.disarmed.walk.forward},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
}

TEST_F(CharacterControllerTests, ArmdIdleToDrawStateAndBackToIdle)
{
    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    tiggerAndExpect<DrawArrowEvent, DrawArrowState>({sut_.animationClipsNames_.drawArrow});
    tiggerAndExpect<AimStopEvent, ArmedIdleState>({sut_.animationClipsNames_.armed.idle});
}

TEST_F(CharacterControllerTests, ArmdIdleToAimStateAndBackToIdle)
{
    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    tiggerAndExpect<DrawArrowEvent, DrawArrowState>({sut_.animationClipsNames_.drawArrow});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectState<AimState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.aimIdle});
    tiggerAndExpect<AimStopEvent, ArmedIdleState>({sut_.animationClipsNames_.armed.idle});
}

TEST_F(CharacterControllerTests, ArrowSimpleShoot)
{
    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    tiggerAndExpect<DrawArrowEvent, DrawArrowState>({sut_.animationClipsNames_.drawArrow});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectState<AimState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.aimIdle});
    tiggerAndExpect<ShootEvent, DrawArrowState>(
        {sut_.animationClipsNames_.drawArrow},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
    tiggerAndExpect<AimStopEvent, ArmedIdleState>({sut_.animationClipsNames_.armed.idle});
}

TEST_F(CharacterControllerTests, AimIdleToAimRotateLeft)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(0));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    tiggerAndExpect<DrawArrowEvent, DrawArrowState>({sut_.animationClipsNames_.drawArrow});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectState<AimState>();
    tiggerAndExpect<RotateLeftEvent, AimRotateState>(
        {sut_.animationClipsNames_.aimIdle, sut_.animationClipsNames_.armed.rotateLeft});
    tiggerAndExpect<RotateRightEvent, AimRotateState>(
        {sut_.animationClipsNames_.aimIdle, sut_.animationClipsNames_.armed.rotateRight});
    tiggerAndExpect<EndRotationEvent, AimState>({sut_.animationClipsNames_.aimIdle});
}

TEST_F(CharacterControllerTests, DrawArrowToDisarmedState)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(0));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    tiggerAndExpect<DrawArrowEvent, DrawArrowState>({sut_.animationClipsNames_.drawArrow});
    tiggerAndExpect<WeaponStateEvent, DisarmedIdleState>(
        {sut_.animationClipsNames_.disarmed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
}

TEST_F(CharacterControllerTests, AimIdleToDisarmedState)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(0));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    tiggerAndExpect<DrawArrowEvent, DrawArrowState>({sut_.animationClipsNames_.drawArrow});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectState<AimState>();

    tiggerAndExpect<WeaponStateEvent, DisarmedIdleState>(
        {sut_.animationClipsNames_.disarmed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
}

TEST_F(CharacterControllerTests, ArrowSimpleShootToDisarmedState)
{
    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    tiggerAndExpect<DrawArrowEvent, DrawArrowState>({sut_.animationClipsNames_.drawArrow});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectState<AimState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.aimIdle});
    tiggerAndExpect<ShootEvent, RecoilState>({sut_.animationClipsNames_.recoilArrow}, {ADVANCED_TIME_TRANSITION_TIME});
    tiggerAndExpect<WeaponStateEvent, DisarmedIdleState>(
        {sut_.animationClipsNames_.disarmed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
}
