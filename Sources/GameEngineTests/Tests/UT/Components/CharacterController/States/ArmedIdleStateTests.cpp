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

TEST_F(CharacterControllerTests, ArmedIdleStateTests_WalkForwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkForwardEvent, ArmedWalkState>({sut_.animationClipsNames_.armed.walk.forward});
}

TEST_F(CharacterControllerTests, ArmedIdleStateTests_WalkBackwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(-DEFAULT_BACKWARD_WALK_SPEED);
    tiggerAndExpect<WalkBackwardEvent, ArmedWalkState>({sut_.animationClipsNames_.armed.walk.backward});
}

TEST_F(CharacterControllerTests, ArmedIdleStateTests_RunForwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<RunForwardEvent, ArmedRunState>({sut_.animationClipsNames_.armed.run.forward});
}

TEST_F(CharacterControllerTests, ArmedIdleStateTests_RunBackwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(-DEFAULT_BACKWARD_RUN_SPEED);
    tiggerAndExpect<RunBackwardEvent, ArmedRunState>({sut_.animationClipsNames_.armed.run.backward});
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

TEST_F(CharacterControllerTests, ArmedIdleStateTests_SprintStartEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStartEvent, ArmedSprintState>({sut_.animationClipsNames_.armed.sprint});
}

TEST_F(CharacterControllerTests,
       DISABLED_ArmedIdleStateTests_UpdateRunForwardEventAndCloseDrawArrowAndBackAsMultiTransitionMiexedToSingle)
{
    prepareState(*this);
    float deltaTime = {0.0001f};

    tiggerAndExpect<RunForwardEvent, ArmedRunState>({sut_.animationClipsNames_.armed.run.forward}, {deltaTime});
    // Run anim not ready
    tiggerAndExpect<DrawArrowEvent, DrawArrowWalkState>(
        {sut_.animationClipsNames_.armed.walk.forward, sut_.animationClipsNames_.drawArrow}, {deltaTime});
    tiggerAndExpect<AimStopEvent, ArmedRunState>({sut_.animationClipsNames_.armed.run.forward}, {deltaTime});
    // Make run animation ready
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    Update(ADVANCED_TIME_TRANSITION_TIME);
}
