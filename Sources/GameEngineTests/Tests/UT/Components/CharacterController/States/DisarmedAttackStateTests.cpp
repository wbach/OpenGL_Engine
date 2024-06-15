#include "../CharacterControllerTests.h"

TEST_F(CharacterControllerTests, DisarmedAttackState_DrawArrowEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<AttackEvent>({sut_.animationClipsNames_.disarmed.attack.front()}, {ADVANCED_TIME_TRANSITION_TIME});

    tiggerAndExpect<DrawArrowEvent>({sut_.animationClipsNames_.equip}, {ADVANCED_TIME_TRANSITION_TIME});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.drawArrow});
}
TEST_F(CharacterControllerTests, DisarmedAttackState_AimStopDuringDrawArrowEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<AttackEvent>({sut_.animationClipsNames_.disarmed.attack.front()}, {ADVANCED_TIME_TRANSITION_TIME});

    tiggerAndExpect<DrawArrowEvent>({sut_.animationClipsNames_.equip}, {ADVANCED_TIME_TRANSITION_TIME});

    tiggerAndExpect<AimStopEvent>({sut_.animationClipsNames_.equip});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.idle});
}

TEST_F(CharacterControllerTests, DisarmedAttackState_WeaponStateEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<AttackEvent>({sut_.animationClipsNames_.disarmed.attack.front()}, {ADVANCED_TIME_TRANSITION_TIME});

    tiggerAndExpect<WeaponStateEvent>({sut_.animationClipsNames_.equip}, {ADVANCED_TIME_TRANSITION_TIME});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
}

TEST_F(CharacterControllerTests, DisarmedAttackState_DeathEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<AttackEvent>({sut_.animationClipsNames_.disarmed.attack.front()}, {ADVANCED_TIME_TRANSITION_TIME});

    tiggerAndExpect<DeathEvent>({sut_.animationClipsNames_.disarmed.death});
}

TEST_F(CharacterControllerTests, DisarmedAttackState_MoveForwardEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<AttackEvent>({sut_.animationClipsNames_.disarmed.attack.front()}, {ADVANCED_TIME_TRANSITION_TIME});

    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveForwardEvent>({sut_.animationClipsNames_.disarmed.run.forward});
    expectRootboneRotation(VECTOR_FORWARD);
}

TEST_F(CharacterControllerTests, DisarmedAttackState_RunBackwardEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<AttackEvent>({sut_.animationClipsNames_.disarmed.attack.front()}, {ADVANCED_TIME_TRANSITION_TIME});

    expectForwardVelocity(-DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveBackwardEvent>({sut_.animationClipsNames_.disarmed.run.forward});
    expectRootboneRotation(VECTOR_BACKWARD);
}

TEST_F(CharacterControllerTests, DisarmedAttackState_MoveLeftEvent)
{
    tiggerAndExpect<AttackEvent>({sut_.animationClipsNames_.disarmed.attack.front()}, {ADVANCED_TIME_TRANSITION_TIME});

    expectVelocity(VECTOR_LEFT, vec3(DEFAULT_RUN_SPEED, 0.0, 0));
    tiggerAndExpect<MoveLeftEvent>({sut_.animationClipsNames_.disarmed.run.forward});
    expectRootboneRotation(VECTOR_LEFT);
}

TEST_F(CharacterControllerTests, DisarmedAttackState_MoveRightEvent)
{
    tiggerAndExpect<AttackEvent>({sut_.animationClipsNames_.disarmed.attack.front()}, {ADVANCED_TIME_TRANSITION_TIME});
    expectVelocity(VECTOR_RIGHT, vec3(DEFAULT_RUN_SPEED, 0.0, 0));
    tiggerAndExpect<MoveRightEvent>({sut_.animationClipsNames_.disarmed.run.forward});
    expectRootboneRotation(VECTOR_RIGHT);
}

TEST_F(CharacterControllerTests, DisarmedAttackState_RotateLeftEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<AttackEvent>({sut_.animationClipsNames_.disarmed.attack.front()}, {ADVANCED_TIME_TRANSITION_TIME});

    expectRotationLeft();
    tiggerAndExpect<RotateLeftEvent>({sut_.animationClipsNames_.disarmed.rotateLeft});
}

TEST_F(CharacterControllerTests, DisarmedAttackState_RotateRightEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<AttackEvent>({sut_.animationClipsNames_.disarmed.attack.front()}, {ADVANCED_TIME_TRANSITION_TIME});

    expectRotationRight();
    tiggerAndExpect<RotateRightEvent>({sut_.animationClipsNames_.disarmed.rotateRight});
}

TEST_F(CharacterControllerTests, DisarmedAttackState_RotateTargetEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<AttackEvent>({sut_.animationClipsNames_.disarmed.attack.front()}, {ADVANCED_TIME_TRANSITION_TIME});

    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent>(RotateTargetEvent{targetRotation.value_},
                                       {sut_.animationClipsNames_.disarmed.rotateLeft});
}
TEST_F(CharacterControllerTests, DisarmedAttackState_EndAttackEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<AttackEvent>({sut_.animationClipsNames_.disarmed.attack.front()}, {ADVANCED_TIME_TRANSITION_TIME});
    tiggerAndExpect<EndAttackEvent>({sut_.animationClipsNames_.disarmed.idle}, {ADVANCED_TIME_TRANSITION_TIME});
}