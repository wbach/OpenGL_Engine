#include <Utils/Time/Timer.h>
#include <gtest/gtest.h>

#include "BaseComponent.h"
#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/Controllers/CharacterController.h"
#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/Components/Physics/SphereShape.h"

using namespace GameEngine;
using namespace GameEngine::Components;
using namespace ::testing;

namespace
{
const int DUMMY_FRAMES             = 4;
const float DUMMY_FRAME_TIME_DELTA = 0.25f;
const float DUMMY_CLIP_LENGTH = DUMMY_FRAME_TIME_DELTA * static_cast<float>(DUMMY_FRAMES);
}  // namespace

struct CharacterControllerTests : public BaseComponentTestSchould
{
    CharacterControllerTests()
        : BaseComponentTestSchould()
        , sut_(context_, obj_)
    {
    }
    virtual void SetUp() override
    {
        EXPECT_CALL(physicsApiMock_, CreateSphereColider(_, _, _)).WillOnce(Return(shapeId));
        EXPECT_CALL(physicsApiMock_, CreateRigidbody(shapeId, _, _, _, _)).WillOnce(Return(rigidbodyid));

        animator_  = &obj_.AddComponent<Animator>();
        rigidbody_ = &obj_.AddComponent<Rigidbody>();
        obj_.AddComponent<SphereShape>().OnAwake();

        rigidbody_->ReqisterFunctions();
        sut_.ReqisterFunctions();
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

        sut_.Init();
        ASSERT_TRUE(sut_.fsm() != nullptr);
        EXPECT_EQ(animator_->getCurrentAnimationName(), sut_.animationClipsNames_.disarmed.idle);
        EXPECT_CALL(physicsApiMock_, RemoveShape(shapeId));
        EXPECT_CALL(physicsApiMock_, RemoveRigidBody(rigidbodyid));
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

    CharacterController sut_;
    Animator* animator_{nullptr};
    Rigidbody* rigidbody_{nullptr};

    GameEngine::Physics::ShapeId shapeId{1};
    GameEngine::Physics::RigidbodyId rigidbodyid{1};
};

TEST_F(CharacterControllerTests, DisarmedState_IdleToRunForward)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillOnce(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillOnce(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, SetVelocityRigidbody(rigidbodyid, vec3(0.0, 0.0, DEFAULT_RUN_SPEED)));

    sut_.fsm()->handle(MoveForwardEvent{});

    context_.time_.deltaTime = DEFAULT_ANIMATION_TRANSITION_TIME + 0.01f;
    animator_->Update();
    sut_.Update();

    EXPECT_EQ(animator_->getCurrentAnimationName(), sut_.animationClipsNames_.disarmed.run.forward);
}

TEST_F(CharacterControllerTests, DisarmedState_IdleToRunBackward)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillOnce(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillOnce(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, SetVelocityRigidbody(rigidbodyid, vec3(0.0, 0.0, -DEFAULT_BACKWARD_RUN_SPEED)));

    sut_.fsm()->handle(MoveBackwardEvent{});

    context_.time_.deltaTime = DEFAULT_ANIMATION_TRANSITION_TIME + 0.01f;
    animator_->Update();
    sut_.Update();

    EXPECT_EQ(animator_->getCurrentAnimationName(), sut_.animationClipsNames_.disarmed.run.backward);
}

TEST_F(CharacterControllerTests, DisarmedState_IdleToRunForwardAndBackward)
{
    context_.time_.deltaTime = DEFAULT_ANIMATION_TRANSITION_TIME + 0.01f;

    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillOnce(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillOnce(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, SetVelocityRigidbody(rigidbodyid, vec3(0.0, 0.0, DEFAULT_RUN_SPEED)));

    sut_.fsm()->handle(MoveForwardEvent{});

    animator_->Update();
    sut_.Update();

    EXPECT_EQ(animator_->getCurrentAnimationName(), sut_.animationClipsNames_.disarmed.run.forward);

    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillOnce(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillOnce(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, SetVelocityRigidbody(rigidbodyid, vec3(0.0, 0.0, -DEFAULT_BACKWARD_RUN_SPEED)));
    sut_.fsm()->handle(MoveBackwardEvent{});

    animator_->Update();
    sut_.Update();

    EXPECT_EQ(animator_->getCurrentAnimationName(), sut_.animationClipsNames_.disarmed.run.backward);

    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillOnce(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillOnce(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, SetVelocityRigidbody(rigidbodyid, vec3(0.0, 0.0, DEFAULT_RUN_SPEED)));
    sut_.fsm()->handle(EndBackwardMoveEvent{});
    animator_->Update();
    sut_.Update();

    EXPECT_EQ(animator_->getCurrentAnimationName(), sut_.animationClipsNames_.disarmed.run.forward);
}

TEST_F(CharacterControllerTests, DisarmedToArmedStateIdle)
{
    sut_.fsm()->handle(WeaponStateEvent{});

    EXPECT_EQ(animator_->getCurrentAnimationName(), sut_.animationClipsNames_.equip);

    //Animation transition : None -> Armed idle state (
    context_.time_.deltaTime = DEFAULT_ANIMATION_TRANSITION_TIME + 0.1f;
    animator_->Update();
    sut_.Update();

    //Animation transition : EquipAnim
    context_.time_.deltaTime = DUMMY_CLIP_LENGTH + 0.1f;
    animator_->Update();
    sut_.Update();

    EXPECT_EQ(animator_->getCurrentAnimationName(), sut_.animationClipsNames_.armed.idle);
}
