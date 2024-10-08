#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    EXPECT_CALL(test.physicsApiMock_, GetVelocity(test.rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(test.physicsApiMock_, GetRotation(test.rigidbodyid)).WillRepeatedly(Return(Rotation().value_));

    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.posture.stand.idle});
    test.tiggerAndExpect<WeaponStateEvent>(
        {test.sut_.animationClipsNames_.armed.posture.stand.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
}
}  // namespace

TEST_F(CharacterControllerTests, ArmedIdleStateTests_DrawArrowEvent)
{
    prepareState(*this);
    tiggerAndExpect<DrawArrowEvent>({sut_.animationClipsNames_.aim.draw});
}

TEST_F(CharacterControllerTests, ArmedIdleStateTests_WeaponStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<WeaponStateEvent>({sut_.animationClipsNames_.disarm});
}

TEST_F(CharacterControllerTests, ArmedIdleStateTests_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent>({sut_.animationClipsNames_.armed.posture.stand.death});
}

TEST_F(CharacterControllerTests, ArmedIdleStateTests_MoveForwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveForwardEvent>({sut_.animationClipsNames_.armed.movement.run.forward});
    expectRootboneRotation(VECTOR_FORWARD);
}

TEST_F(CharacterControllerTests, ArmedIdleStateTests_MoveBackwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(-DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveBackwardEvent>({sut_.animationClipsNames_.armed.movement.run.forward});
    expectRootboneRotation(VECTOR_BACKWARD);
}

TEST_F(CharacterControllerTests, ArmedIdleStateTests_MoveLeftEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_LEFT, vec3(DEFAULT_RUN_SPEED, 0.0, 0));
    tiggerAndExpect<MoveLeftEvent>({sut_.animationClipsNames_.armed.movement.run.forward});
    expectRootboneRotation(VECTOR_LEFT);
}

TEST_F(CharacterControllerTests, ArmedIdleStateTests_MoveRightEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_RIGHT, vec3(DEFAULT_RUN_SPEED, 0.0, 0));
    tiggerAndExpect<MoveRightEvent>({sut_.animationClipsNames_.armed.movement.run.forward});
    expectRootboneRotation(VECTOR_RIGHT);
}

TEST_F(CharacterControllerTests, ArmedIdleStateTests_RotateLeftEvent)
{
    prepareState(*this);
    expectRotationLeft();
    tiggerAndExpect<RotateLeftEvent>({sut_.animationClipsNames_.armed.posture.stand.rotate.left});
}

TEST_F(CharacterControllerTests, ArmedIdleStateTests_RotateRightEvent)
{
    prepareState(*this);
    expectRotationRight();
    tiggerAndExpect<RotateRightEvent>({sut_.animationClipsNames_.armed.posture.stand.rotate.right});
}

TEST_F(CharacterControllerTests, ArmedIdleStateTests_RotateTargetEvent)
{
    prepareState(*this);

    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent>(RotateTargetEvent{targetRotation.value_},
                                       {sut_.animationClipsNames_.armed.posture.stand.rotate.left});
}

TEST_F(CharacterControllerTests, ArmedIdleStateTests_UpdateRunForwardEventAndCloseDrawArrowAndBackAsMultiTransitionMiexedToSingle)
{
    prepareState(*this);
    float deltaTime = {0.0001f};

    tiggerAndExpect<MoveForwardEvent>({sut_.animationClipsNames_.armed.movement.run.forward}, {deltaTime});
    // Run anim not ready
    tiggerAndExpect<DrawArrowEvent>({sut_.animationClipsNames_.armed.movement.walk.forward, sut_.animationClipsNames_.aim.draw},
                                    {deltaTime});
    tiggerAndExpect<AimStopEvent>({sut_.animationClipsNames_.armed.movement.run.forward}, {deltaTime, deltaTime});
    // Make run animation ready
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    Update(ADVANCED_TIME_TRANSITION_TIME);
}

TEST_F(CharacterControllerTests, ArmedIdleStateTests_CrouchChangeStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<CrouchChangeStateEvent>({sut_.animationClipsNames_.armed.posture.crouched.idle},
                                            {ADVANCED_TIME_TRANSITION_TIME});
}
