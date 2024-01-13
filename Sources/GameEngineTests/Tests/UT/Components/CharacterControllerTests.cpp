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

        animator_->AddAnimationClip(Animation::AnimationClip(clips.disarmed.idle));
        animator_->AddAnimationClip(Animation::AnimationClip(clips.disarmed.run.forward));
        animator_->AddAnimationClip(Animation::AnimationClip(clips.disarmed.run.backward));
        sut_.equipTimeStamp  = 0.0f;
        sut_.disarmTimeStamp = 0.0f;

        sut_.Init();
        ASSERT_TRUE(sut_.fsm() != nullptr);
        EXPECT_EQ(animator_->getCurrentAnimationName(), sut_.animationClipsNames_.disarmed.idle);
        EXPECT_CALL(physicsApiMock_, RemoveShape(shapeId));
        EXPECT_CALL(physicsApiMock_, RemoveRigidBody(rigidbodyid));
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

    context_.time_.deltaTime = 0.2f;
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

    context_.time_.deltaTime = 0.2f;
    animator_->Update();
    sut_.Update();

    EXPECT_EQ(animator_->getCurrentAnimationName(), sut_.animationClipsNames_.disarmed.run.backward);
}
