#include "../CharacterControllerTests.h"
#include "Components/Controllers/CharacterController/CharacterControllerEvents.h"
#include "Components/Controllers/CharacterController/MoveSpeed.h"
#include "Types.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    EXPECT_CALL(test.physicsApiMock_, GetVelocity(test.rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(test.physicsApiMock_, GetRotation(test.rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    test.expectNoMove();

    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.idle});
    test.tiggerAndExpect<WeaponStateEvent>(
        {test.sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
    test.expectRotationLeft();
    test.tiggerAndExpect<RotateLeftEvent>({test.sut_.animationClipsNames_.armed.rotateLeft});
    test.tiggerAndExpect<AttackEvent>({test.armedAttackClip1->name, test.sut_.animationClipsNames_.armed.rotateLeft});
}
}  // namespace

TEST_F(CharacterControllerTests, ArmedAttackAndRotateState_DrawArrowEvent)
{
    prepareState(*this);
    expectRotationLeft();
    tiggerAndExpect<GameEngine::DrawArrowEvent>({armedAttackClip1->name, sut_.animationClipsNames_.armed.rotateLeft});

    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectRotationLeft(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.drawArrow, sut_.animationClipsNames_.armed.rotateLeft});
}
TEST_F(CharacterControllerTests, ArmedAttackAndRotateState_AimStop)
{
    prepareState(*this);
    expectRotationLeft();
    tiggerAndExpect<GameEngine::DrawArrowEvent>({armedAttackClip1->name, sut_.animationClipsNames_.armed.rotateLeft});
    expectAnyRotation();
    tiggerAndExpect<GameEngine::AimStopEvent>({armedAttackClip1->name, sut_.animationClipsNames_.armed.rotateLeft}, {0});
    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectRotationLeft(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.rotateLeft});
}
//TEST_F(CharacterControllerTests, DISABLED_ArmedAttackAndRotateState_CrouchChangeStateEvent)
//{
//    prepareState(*this);
//    tiggerAndExpect<GameEngine::CrouchChangeStateEvent>(
//        {armedAttackClip1->name, sut_.animationClipsNames_.armed.rotateLeft});

//    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
//    Update(ADVANCED_TIME_CLIP_TIME);
//    expectRotationLeft(ADVANCED_TIME_TRANSITION_TIME);
//    Update(ADVANCED_TIME_TRANSITION_TIME);

//    // expectAnimsToBeSet({sut_.animationClipsNames_.armed.crouch.rotateLeft});
//}
TEST_F(CharacterControllerTests, ArmedAttackAndRotateState_MoveForwardEvent)
{
    prepareState(*this);
    expectRotationLeft();
    expectForwardVelocity(GameEngine::Components::DEFAULT_RUN_SPEED);
    tiggerAndExpect<GameEngine::MoveForwardEvent>({armedAttackClip1->name, sut_.animationClipsNames_.armed.run.forward});
}
TEST_F(CharacterControllerTests, ArmedAttackAndRotateState_MoveBackwardEvent)
{
    prepareState(*this);
    expectAnyRotation();
    expectForwardVelocity(-GameEngine::Components::DEFAULT_RUN_SPEED);
    tiggerAndExpect<GameEngine::MoveBackwardEvent>({armedAttackClip1->name, sut_.animationClipsNames_.armed.run.forward});
    expectRootboneRotation(VECTOR_BACKWARD);
}
TEST_F(CharacterControllerTests, ArmedAttackAndRotateState_MoveLeftEvent)
{
    prepareState(*this);
    expectAnyRotation();
    expectVelocity(VECTOR_LEFT, vec3(GameEngine::Components::DEFAULT_RUN_SPEED, 0.0, 0.0));
    tiggerAndExpect<GameEngine::MoveLeftEvent>({armedAttackClip1->name, sut_.animationClipsNames_.armed.run.forward});
    expectRootboneRotation(VECTOR_LEFT);
}
TEST_F(CharacterControllerTests, ArmedAttackAndRotateState_MoveRighEvent)
{
    prepareState(*this);
    expectAnyRotation();
    expectVelocity(VECTOR_RIGHT, vec3(GameEngine::Components::DEFAULT_RUN_SPEED, 0.0, 0.0));
    tiggerAndExpect<GameEngine::MoveRightEvent>({armedAttackClip1->name, sut_.animationClipsNames_.armed.run.forward});
    expectRootboneRotation(VECTOR_RIGHT);
}
TEST_F(CharacterControllerTests, ArmedAttackAndRotateState_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent>({sut_.animationClipsNames_.armed.death});
}
TEST_F(CharacterControllerTests, ArmedAttackAndRotateState_AttackEvent)
{
    prepareState(*this);
    tiggerAndExpect<AttackEvent>(
        {sut_.animationClipsNames_.armed.attack.front().name, sut_.animationClipsNames_.armed.rotateLeft});

    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectRotationLeft(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.attack[1].name, sut_.animationClipsNames_.armed.rotateLeft});

    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectRotationLeft(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.rotateLeft});
}

TEST_F(CharacterControllerTests, ArmedAttackAndRotateState_EndRotationEvent)
{
    prepareState(*this);
    tiggerAndExpect<GameEngine::EndRotationEvent>({armedAttackClip1->name});
}

TEST_F(CharacterControllerTests, ArmedAttackAndRotateState_RotateLeftEvent)
{
    prepareState(*this);
    tiggerAndExpect<RotateLeftEvent>({armedAttackClip1->name, sut_.animationClipsNames_.armed.rotateLeft});
}

TEST_F(CharacterControllerTests, ArmedAttackAndRotateState_RotateRightEvent)
{
    prepareState(*this);
    expectRotationRight();
    tiggerAndExpect<RotateRightEvent>({armedAttackClip1->name, sut_.animationClipsNames_.armed.rotateRight});
}

TEST_F(CharacterControllerTests, ArmedAttackAndRotateState_RotateTargetEvent)
{
    prepareState(*this);

    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent>(RotateTargetEvent{targetRotation.value_},
                                       {armedAttackClip1->name, sut_.animationClipsNames_.armed.rotateLeft});
}
