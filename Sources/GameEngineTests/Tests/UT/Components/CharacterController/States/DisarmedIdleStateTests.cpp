#include "../CharacterControllerTests.h"

TEST_F(CharacterControllerTests, DisarmedIdleState_DrawArrowEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    tiggerAndExpect<DrawArrowEvent>({sut_.animationClipsNames_.equip}, {ADVANCED_TIME_TRANSITION_TIME});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.aim.draw});
}
TEST_F(CharacterControllerTests, DisarmedIdleState_AimStopDuringDrawArrowEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    tiggerAndExpect<DrawArrowEvent>({sut_.animationClipsNames_.equip}, {ADVANCED_TIME_TRANSITION_TIME});

    tiggerAndExpect<AimStopEvent>({sut_.animationClipsNames_.equip});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, DisarmedIdleState_WeaponStateEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    tiggerAndExpect<WeaponStateEvent>({sut_.animationClipsNames_.equip}, {ADVANCED_TIME_TRANSITION_TIME});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
}

TEST_F(CharacterControllerTests, DisarmedIdleState_DeathEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    tiggerAndExpect<DeathEvent>({sut_.animationClipsNames_.disarmed.posture.stand.death});
}

TEST_F(CharacterControllerTests, DisarmedIdleState_MoveForwardEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveForwardEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
    expectRootboneRotation(VECTOR_FORWARD);
}

TEST_F(CharacterControllerTests, DisarmedIdleState_RunBackwardEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    expectForwardVelocity(-DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveBackwardEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
    expectRootboneRotation(VECTOR_BACKWARD);
}

TEST_F(CharacterControllerTests, DisarmedIdleState_MoveLeftEvent)
{
    expectVelocity(VECTOR_LEFT, vec3(DEFAULT_RUN_SPEED, 0.0, 0));
    tiggerAndExpect<MoveLeftEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
    expectRootboneRotation(VECTOR_LEFT);
}

TEST_F(CharacterControllerTests, DisarmedIdleState_MoveRightEvent)
{
    expectVelocity(VECTOR_RIGHT, vec3(DEFAULT_RUN_SPEED, 0.0, 0));
    tiggerAndExpect<MoveRightEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
    expectRootboneRotation(VECTOR_RIGHT);
}

TEST_F(CharacterControllerTests, DisarmedIdleState_RotateLeftEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    expectRotationLeft();
    tiggerAndExpect<RotateLeftEvent>({sut_.animationClipsNames_.disarmed.posture.stand.rotate.left});
}

TEST_F(CharacterControllerTests, DisarmedIdleState_RotateRightEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    expectRotationRight();
    tiggerAndExpect<RotateRightEvent>({sut_.animationClipsNames_.disarmed.posture.stand.rotate.right});
}

TEST_F(CharacterControllerTests, DisarmedIdleState_RotateTargetEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});

    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent>(RotateTargetEvent{targetRotation.value_},
                                       {sut_.animationClipsNames_.disarmed.posture.stand.rotate.right});
}

TEST_F(CharacterControllerTests, DisarmedIdleState_AttackEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    tiggerAndExpect<AttackEvent>({sut_.animationClipsNames_.disarmed.attack.front().name}, {ADVANCED_TIME_TRANSITION_TIME});
}

TEST_F(CharacterControllerTests, DisarmedIdleState_CrouchChangeStateEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    tiggerAndExpect<CrouchChangeStateEvent>({sut_.animationClipsNames_.disarmed.posture.crouched.idle},
                                            {ADVANCED_TIME_TRANSITION_TIME});
}
