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
}
}  // namespace

TEST_F(CharacterControllerTests, ArmedIdleStateTests_DrawArrowEvent)
{
    prepareState(*this);
    tiggerAndExpect<DrawArrowEvent, DrawArrowState>({sut_.animationClipsNames_.drawArrow});
}

TEST_F(CharacterControllerTests, ArmedIdleStateTests_WeaponStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<WeaponStateEvent, IdleArmedChangeState>({sut_.animationClipsNames_.disarm});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectState<DisarmedIdleState>();
}

TEST_F(CharacterControllerTests, ArmedIdleStateTests_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent, DeathState>({sut_.animationClipsNames_.armed.death});
}

TEST_F(CharacterControllerTests, ArmedIdleStateTests_MoveForwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveForwardEvent, ArmedWalkState>({sut_.animationClipsNames_.armed.run.forward});
    expectRootboneRotation(VECTOR_FORWARD);
}

TEST_F(CharacterControllerTests, ArmedIdleStateTests_MoveBackwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(-DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveBackwardEvent, ArmedWalkState>({sut_.animationClipsNames_.armed.run.forward});
    expectRootboneRotation(VECTOR_BACKWARD);
}

TEST_F(CharacterControllerTests, ArmedIdleStateTests_MoveLeftEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_LEFT, vec3(DEFAULT_RUN_SPEED, 0.0, 0));
    tiggerAndExpect<MoveLeftEvent, ArmedWalkState>({sut_.animationClipsNames_.armed.run.forward});
    expectRootboneRotation(VECTOR_LEFT);
}

TEST_F(CharacterControllerTests, ArmedIdleStateTests_MoveRightEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_RIGHT, vec3(DEFAULT_RUN_SPEED, 0.0, 0));
    tiggerAndExpect<MoveRightEvent, ArmedWalkState>({sut_.animationClipsNames_.armed.run.forward});
    expectRootboneRotation(VECTOR_RIGHT);
}

TEST_F(CharacterControllerTests, ArmedIdleStateTests_RotateLeftEvent)
{
    prepareState(*this);
    expectRotationLeft();
    tiggerAndExpect<RotateLeftEvent, ArmedRotateState>({sut_.animationClipsNames_.armed.rotateLeft});
}

TEST_F(CharacterControllerTests, ArmedIdleStateTests_RotateRightEvent)
{
    prepareState(*this);
    expectRotationRight();
    tiggerAndExpect<RotateRightEvent, ArmedRotateState>({sut_.animationClipsNames_.armed.rotateRight});
}

TEST_F(CharacterControllerTests, ArmedIdleStateTests_RotateTargetEvent)
{
    prepareState(*this);

    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent, ArmedRotateState>(RotateTargetEvent{targetRotation.value_},
                                                         {sut_.animationClipsNames_.armed.rotateLeft});
}

TEST_F(CharacterControllerTests,
       ArmedIdleStateTests_UpdateRunForwardEventAndCloseDrawArrowAndBackAsMultiTransitionMiexedToSingle)
{
    prepareState(*this);
    float deltaTime = {0.0001f};

    tiggerAndExpect<MoveForwardEvent, ArmedRunState>({sut_.animationClipsNames_.armed.run.forward}, {deltaTime});
    // Run anim not ready
    tiggerAndExpect<DrawArrowEvent, DrawArrowWalkState>(
        {sut_.animationClipsNames_.armed.walk.forward, sut_.animationClipsNames_.drawArrow}, {deltaTime});
    tiggerAndExpect<AimStopEvent, ArmedRunState>({sut_.animationClipsNames_.armed.run.forward}, {deltaTime, deltaTime});
    // Make run animation ready
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    Update(ADVANCED_TIME_TRANSITION_TIME);
}
