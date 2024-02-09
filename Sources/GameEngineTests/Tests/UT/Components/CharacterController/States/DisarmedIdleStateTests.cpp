#include "../CharacterControllerTests.h"

TEST_F(CharacterControllerTests, DisarmedIdleState_DrawArrowEvent)
{
    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<DrawArrowEvent, IdleArmedChangeState>({sut_.animationClipsNames_.equip},
                                                          {ADVANCED_TIME_TRANSITION_TIME});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectState<DrawArrowState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.drawArrow});
}
TEST_F(CharacterControllerTests, DisarmedIdleState_AimStopDuringDrawArrowEvent)
{
    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<DrawArrowEvent, IdleArmedChangeState>({sut_.animationClipsNames_.equip},
                                                          {ADVANCED_TIME_TRANSITION_TIME});

    tiggerAndExpect<AimStopEvent, IdleArmedChangeState>({sut_.animationClipsNames_.equip});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectState<ArmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.idle});
}

TEST_F(CharacterControllerTests, DisarmedIdleState_WeaponStateEvent)
{
    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<WeaponStateEvent, IdleArmedChangeState>({sut_.animationClipsNames_.equip},
                                                            {ADVANCED_TIME_TRANSITION_TIME});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectState<ArmedIdleState>();
}

TEST_F(CharacterControllerTests, DisarmedIdleState_DeathEvent)
{
    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<DeathEvent, DeathState>({sut_.animationClipsNames_.disarmed.death});
}

TEST_F(CharacterControllerTests, DisarmedIdleState_RunForwardEvent)
{
    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<RunForwardEvent, DisarmedRunState>({sut_.animationClipsNames_.disarmed.run.forward});
}

TEST_F(CharacterControllerTests, DisarmedIdleState_RunBackwardEvent)
{
    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    expectForwardVelocity(-DEFAULT_BACKWARD_RUN_SPEED);
    tiggerAndExpect<RunBackwardEvent, DisarmedRunState>({sut_.animationClipsNames_.disarmed.run.backward});
}

TEST_F(CharacterControllerTests, DisarmedIdleState_WalkForwardEvent)
{
    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkForwardEvent, DisarmedWalkState>({sut_.animationClipsNames_.disarmed.walk.forward});
}

TEST_F(CharacterControllerTests, DisarmedIdleState_WalkBackwardEvent)
{
    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    expectForwardVelocity(-DEFAULT_BACKWARD_WALK_SPEED);
    tiggerAndExpect<WalkBackwardEvent, DisarmedWalkState>({sut_.animationClipsNames_.disarmed.walk.backward});
}

TEST_F(CharacterControllerTests, DisarmedIdleState_SprintStartEvent)
{
    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStartEvent, DisarmedSprintState>({sut_.animationClipsNames_.disarmed.sprint});
}

TEST_F(CharacterControllerTests, DisarmedIdleState_RotateLeftEvent)
{
    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    expectRotationLeft();
    tiggerAndExpect<RotateLeftEvent, DisarmedRotateState>({sut_.animationClipsNames_.disarmed.rotateLeft});
}

TEST_F(CharacterControllerTests, DisarmedIdleState_RotateRightEvent)
{
    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    expectRotationRight();
    tiggerAndExpect<RotateRightEvent, DisarmedRotateState>({sut_.animationClipsNames_.disarmed.rotateRight});
}

TEST_F(CharacterControllerTests, DisarmedIdleState_RotateTargetEvent)
{
    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});

    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent, DisarmedRotateState>(RotateTargetEvent{targetRotation.value_},
                                                            {sut_.animationClipsNames_.disarmed.rotateLeft});
}

