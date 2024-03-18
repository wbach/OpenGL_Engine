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

TEST_F(CharacterControllerTests, DisarmedIdleState_MoveForwardEvent)
{
    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveForwardEvent, DisarmedRunState>({sut_.animationClipsNames_.disarmed.run.forward});
    expectRootboneRotation(VECTOR_FORWARD);
}

TEST_F(CharacterControllerTests, DisarmedIdleState_RunBackwardEvent)
{
    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    expectForwardVelocity(-DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveBackwardEvent, DisarmedRunState>({sut_.animationClipsNames_.disarmed.run.forward});
    expectRootboneRotation(VECTOR_BACKWARD);
}

TEST_F(CharacterControllerTests, DisarmedIdleState_MoveLeftEvent)
{
    expectState<DisarmedIdleState>();
    expectVelocity(VECTOR_LEFT, vec3(DEFAULT_RUN_SPEED, 0.0, 0));
    tiggerAndExpect<MoveLeftEvent, ArmedRunState>({sut_.animationClipsNames_.armed.run.forward});
    expectRootboneRotation(VECTOR_LEFT);
}

TEST_F(CharacterControllerTests, DisarmedIdleState_MoveRightEvent)
{
    expectState<DisarmedIdleState>();
    expectVelocity(VECTOR_RIGHT, vec3(DEFAULT_RUN_SPEED, 0.0, 0));
    tiggerAndExpect<MoveRightEvent, ArmedRunState>({sut_.animationClipsNames_.armed.run.forward});
    expectRootboneRotation(VECTOR_RIGHT);
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

TEST_F(CharacterControllerTests, DISABLED_DisarmedIdleState_MoveLeftEvent)
{
    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<MoveLeftEvent, DisarmedRunState>({sut_.animationClipsNames_.disarmed.run.forward});
}

TEST_F(CharacterControllerTests, DISABLED_DisarmedIdleState_MoveRightEvent)
{
    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<MoveRightEvent, DisarmedRunState>({sut_.animationClipsNames_.disarmed.run.forward});
}
