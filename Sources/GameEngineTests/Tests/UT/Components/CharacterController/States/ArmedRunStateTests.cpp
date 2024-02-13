#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    EXPECT_CALL(test.physicsApiMock_, GetVelocity(test.rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(test.physicsApiMock_, GetRotation(test.rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    test.expectForwardVelocity(DEFAULT_RUN_SPEED);

    test.expectState<DisarmedIdleState>();
    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.idle});
    test.tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {test.sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    test.tiggerAndExpect<RunForwardEvent, ArmedRunState>({test.sut_.animationClipsNames_.armed.run.forward});
}
}  // namespace

TEST_F(CharacterControllerTests, ArmedRun_DrawArrowEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<DrawArrowEvent, DrawArrowWalkState>(
        {sut_.animationClipsNames_.drawArrow, sut_.animationClipsNames_.armed.walk.forward});
}

TEST_F(CharacterControllerTests, ArmedRun_RunForwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(-DEFAULT_BACKWARD_RUN_SPEED);
    tiggerAndExpect<RunBackwardEvent, ArmedRunState>({sut_.animationClipsNames_.armed.run.backward});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<RunForwardEvent, ArmedRunState>({sut_.animationClipsNames_.armed.run.forward});
}

TEST_F(CharacterControllerTests, ArmedRun_RunBackwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(-DEFAULT_BACKWARD_RUN_SPEED);
    tiggerAndExpect<RunBackwardEvent, ArmedRunState>({sut_.animationClipsNames_.armed.run.backward});
}

TEST_F(CharacterControllerTests, ArmedRun_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent, DeathState>({sut_.animationClipsNames_.armed.death});
}

TEST_F(CharacterControllerTests, ArmedRun_WalkChangeStateEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent, ArmedWalkState>({sut_.animationClipsNames_.armed.walk.forward});
}

TEST_F(CharacterControllerTests, ArmedRun_WeaponStateEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<WeaponStateEvent, RunArmedChangeState>(
        {sut_.animationClipsNames_.disarmed.run.forward, sut_.animationClipsNames_.disarm});
}

TEST_F(CharacterControllerTests, ArmedRun_EndForwardMoveEvent)
{
    prepareState(*this);
    tiggerAndExpect<EndForwardMoveEvent, ArmedIdleState>({sut_.animationClipsNames_.armed.idle});
}

TEST_F(CharacterControllerTests, ArmedRun_EndBackwardMoveEvent)
{
    prepareState(*this);
    expectForwardVelocity(-DEFAULT_BACKWARD_RUN_SPEED);
    tiggerAndExpect<RunBackwardEvent, ArmedRunState>({sut_.animationClipsNames_.armed.run.backward});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<EndBackwardMoveEvent, ArmedRunState>({sut_.animationClipsNames_.armed.run.forward});
}

TEST_F(CharacterControllerTests, ArmedRun_RotateLeftEvent)
{
    prepareState(*this);
    expectRotationLeft();
    tiggerAndExpect<RotateLeftEvent, ArmedRunAndRotateState>({sut_.animationClipsNames_.armed.run.forward});
}

TEST_F(CharacterControllerTests, ArmedRun_RotateRightEvent)
{
    prepareState(*this);
    expectRotationRight();
    tiggerAndExpect<RotateRightEvent, ArmedRunAndRotateState>({sut_.animationClipsNames_.armed.run.forward});
}

TEST_F(CharacterControllerTests, ArmedRun_RotateTargetEvent)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent, ArmedRunAndRotateState>(RotateTargetEvent{targetRotation.value_},
                                                               {sut_.animationClipsNames_.armed.run.forward});
}

TEST_F(CharacterControllerTests, ArmedRun_SprintStateChangeEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent, ArmedSprintState>({sut_.animationClipsNames_.armed.sprint});
}

TEST_F(CharacterControllerTests, DISABLED_ArmedRunState_UpdateToDrawArrowAndBackAsMultiTransitionMiexedToSingle)
{
    prepareState(*this);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    float deltaTime = { 0.0001f };

    for (int i = 0; i < 10; i++)
    {
        expectForwardVelocity(DEFAULT_WALK_SPEED);
        tiggerAndExpect<DrawArrowEvent, DrawArrowWalkState>(
            { sut_.animationClipsNames_.armed.walk.forward, sut_.animationClipsNames_.drawArrow }, { deltaTime });
        expectForwardVelocity(DEFAULT_WALK_SPEED);
        Update(deltaTime);
        tiggerAndExpect<AimStopEvent, ArmedRunState>({ sut_.animationClipsNames_.armed.run.forward }, { deltaTime });
        expectForwardVelocity(DEFAULT_RUN_SPEED);
        Update(deltaTime);
    }
}
