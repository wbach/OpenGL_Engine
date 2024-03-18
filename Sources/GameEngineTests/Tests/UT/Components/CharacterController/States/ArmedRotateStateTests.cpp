#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    EXPECT_CALL(test.physicsApiMock_, GetVelocity(test.rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(test.physicsApiMock_, GetRotation(test.rigidbodyid)).WillRepeatedly(Return(Rotation().value_));

    test.expectState<DisarmedIdleState>();
    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.idle});
    test.tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {test.sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    test.expectRotationRight(ADVANCED_TIME_TRANSITION_TIME);
    test.tiggerAndExpect<RotateRightEvent, ArmedRotateState>({test.sut_.animationClipsNames_.armed.rotateRight});
}
}  // namespace

TEST_F(CharacterControllerTests, ArmedRotate_DrawArrowEvent)
{
    prepareState(*this);
    tiggerAndExpect<DrawArrowEvent, DrawArrowRotateState>(
        {sut_.animationClipsNames_.drawArrow, sut_.animationClipsNames_.armed.rotateRight});
}

TEST_F(CharacterControllerTests, ArmedRotate_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent, DeathState>({sut_.animationClipsNames_.armed.death});
}

TEST_F(CharacterControllerTests, ArmedRotate_WeaponStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<WeaponStateEvent, RotateArmedChangeState>(
        {sut_.animationClipsNames_.disarmed.rotateRight, sut_.animationClipsNames_.disarm});
}

TEST_F(CharacterControllerTests, ArmedRotate_MoveForwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveForwardEvent, ArmedRunAndRotateState>({sut_.animationClipsNames_.armed.run.forward});
}

TEST_F(CharacterControllerTests, ArmedRotate_RunBackwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(-DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveBackwardEvent, ArmedRunAndRotateState>({sut_.animationClipsNames_.armed.run.backward});
}

TEST_F(CharacterControllerTests, ArmedRotate_MoveLeftEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_LEFT, vec3(DEFAULT_RUN_SPEED, 0.0, 0));
    tiggerAndExpect<MoveLeftEvent, ArmedRunAndRotateState>({sut_.animationClipsNames_.armed.run.forward});
    expectRootboneRotation(VECTOR_LEFT);
}

TEST_F(CharacterControllerTests, ArmedRotate_MoveRightEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_RIGHT, vec3(DEFAULT_RUN_SPEED, 0.0, 0));
    tiggerAndExpect<MoveRightEvent, ArmedRunAndRotateState>({sut_.animationClipsNames_.armed.run.forward});
    expectRootboneRotation(VECTOR_RIGHT);
}

TEST_F(CharacterControllerTests, ArmedRotate_RotateLeftEvent)
{
    prepareState(*this);
    expectRotationRight();
    tiggerAndExpect<RotateRightEvent, ArmedRotateState>({sut_.animationClipsNames_.armed.rotateRight});
    expectRotationLeft();
    tiggerAndExpect<RotateLeftEvent, ArmedRotateState>({sut_.animationClipsNames_.armed.rotateLeft});
}

TEST_F(CharacterControllerTests, ArmedRotate_RotateTargetEvent)
{
    prepareState(*this);
    expectRotationRight();
    tiggerAndExpect<RotateRightEvent, ArmedRotateState>({sut_.animationClipsNames_.armed.rotateRight});

    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent, ArmedRotateState>(RotateTargetEvent{targetRotation.value_},
                                                         {sut_.animationClipsNames_.armed.rotateLeft});
}

TEST_F(CharacterControllerTests, ArmedRotate_EndRotationEvent)
{
    prepareState(*this);
    tiggerAndExpect<EndRotationEvent, ArmedIdleState>({sut_.animationClipsNames_.armed.idle});
}
