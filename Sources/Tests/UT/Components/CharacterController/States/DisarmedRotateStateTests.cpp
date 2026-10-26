#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    EXPECT_CALL(test.physicsApiMock_, GetVelocity(test.rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(test.physicsApiMock_, GetRotation(test.rigidbodyid)).WillRepeatedly(Return(Rotation().value_));

    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.posture.stand.idle});
    test.expectRotationLeft();
    test.tiggerAndExpect<RotateLeftEvent>({test.sut_.animationClipsNames_.disarmed.posture.stand.rotate.left});
}
}  // namespace

TEST_F(CharacterControllerTests, DisarmedRotateState_DrawArrowEvent)
{
    prepareState(*this);
    expectRotationLeft(ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<DrawArrowEvent>(
        {sut_.animationClipsNames_.armed.posture.stand.rotate.left, sut_.animationClipsNames_.equip.front()},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_TRANSITION_TIME});
    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectRotationLeft(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.posture.stand.rotate.left, sut_.animationClipsNames_.aim.draw});
}

TEST_F(CharacterControllerTests, DisarmedRotateState_AimStopDuringDrawArrowEvent)
{
    prepareState(*this);
    expectRotationLeft(ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<DrawArrowEvent>(
        {sut_.animationClipsNames_.armed.posture.stand.rotate.left, sut_.animationClipsNames_.equip.front()},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_TRANSITION_TIME});

    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    tiggerAndExpect<AimStopEvent>({sut_.animationClipsNames_.armed.posture.stand.rotate.left},
                                  {ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_CLIP_TIME});

    // Extra check
    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.posture.stand.rotate.left});
}

TEST_F(CharacterControllerTests, DisarmedRotateState_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent>({sut_.animationClipsNames_.disarmed.posture.stand.death});
}

TEST_F(CharacterControllerTests, DisarmedRotateState_WeaponStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<WeaponStateEvent>(
        {sut_.animationClipsNames_.armed.posture.stand.rotate.left, sut_.animationClipsNames_.equip.front()});
    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectRotationLeft();
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.posture.stand.rotate.left});
}

TEST_F(CharacterControllerTests, DisarmedRotateState_MoveForwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveForwardEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
}

TEST_F(CharacterControllerTests, DisarmedRotateState_MoveBackwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(-DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveBackwardEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
    expectRotationLeft(DEFAULT_MOVING_CHANGE_DIR_SPEED);
    expectRootboneRotation(VECTOR_BACKWARD);
}

TEST_F(CharacterControllerTests, DisarmedRotateState_MoveLeftEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_LEFT, vec3(DEFAULT_RUN_SPEED, 0.0, 0));
    tiggerAndExpect<MoveLeftEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
    expectRotationLeft(DEFAULT_MOVING_CHANGE_DIR_SPEED);
    expectRootboneRotation(VECTOR_LEFT);
}

TEST_F(CharacterControllerTests, DisarmedRotateState_MoveRightEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_RIGHT, vec3(DEFAULT_RUN_SPEED, 0.0, 0));
    tiggerAndExpect<MoveRightEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
    expectRotationLeft(DEFAULT_MOVING_CHANGE_DIR_SPEED);
    expectRootboneRotation(VECTOR_RIGHT);
}

TEST_F(CharacterControllerTests, DisarmedRotateState_RotateLeftEvent)
{
    prepareState(*this);
    tiggerAndExpect<RotateLeftEvent>({sut_.animationClipsNames_.disarmed.posture.stand.rotate.left});
}

TEST_F(CharacterControllerTests, DisarmedRotateState_RotateRightEvent)
{
    prepareState(*this);
    expectRotationRight();
    tiggerAndExpect<RotateRightEvent>({sut_.animationClipsNames_.disarmed.posture.stand.rotate.right});
}

TEST_F(CharacterControllerTests, DisarmedRotateState_RotateTargetEvent)
{
    prepareState(*this);

    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent>(RotateTargetEvent{targetRotation.value_},
                                       {sut_.animationClipsNames_.disarmed.posture.stand.rotate.right});
}

TEST_F(CharacterControllerTests, DisarmedRotateState_EndRotationEvent)
{
    prepareState(*this);
    tiggerAndExpect<EndRotationEvent>({sut_.animationClipsNames_.disarmed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, DisarmedRotateState_WaitForEndRotationEvent_RotateToTarget)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    const auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);

    tiggerAndExpect<RotateTargetEvent>(RotateTargetEvent{targetRotation.value_},
                                       {sut_.animationClipsNames_.disarmed.posture.stand.rotate.right});

    Update(1.f);  // Duza wartosc aby napewno target zostal osiagniety
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, DisarmedRotateState_MoveEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
}

TEST_F(CharacterControllerTests, DisarmedRotateState_WalkChangeStateEvent)
{
    prepareState(*this);
    expectIgnoreEvent<WalkChangeStateEvent>();
}

TEST_F(CharacterControllerTests, DisarmedRotateState_SprintStateChangeEvent)
{
    prepareState(*this);
    expectIgnoreEvent<SprintStateChangeEvent>();
}

TEST_F(CharacterControllerTests, DisarmedRotateState_EndForwardMoveEvent)
{
    prepareState(*this);
    expectIgnoreEvent<EndForwardMoveEvent>();
}

TEST_F(CharacterControllerTests, DisarmedRotateState_EndBackwardMoveEvent)
{
    prepareState(*this);
    expectIgnoreEvent<EndBackwardMoveEvent>();
}

TEST_F(CharacterControllerTests, DisarmedRotateState_EndMoveLeftEvent)
{
    prepareState(*this);
    expectIgnoreEvent<EndMoveLeftEvent>();
}

TEST_F(CharacterControllerTests, DisarmedRotateState_EndMoveRightEvent)
{
    prepareState(*this);
    expectIgnoreEvent<EndMoveRightEvent>();
}

TEST_F(CharacterControllerTests, DisarmedRotateState_JumpEvent)
{
    prepareState(*this);
    expectVelocity(vec3(0, 1, 0), vec3(DEFAULT_JUMP_POWER));
    handleEvent(JumpEvent{DEFAULT_JUMP_POWER});
    tiggerAndExpect<JumpConfirmEvent>({sut_.animationClipsNames_.disarmed.jump});
}

TEST_F(CharacterControllerTests, DisarmedRotateState_JumpConfirmEvent)
{
    prepareState(*this);
    expectIgnoreEvent<JumpConfirmEvent>();
}

TEST_F(CharacterControllerTests, DisarmedRotateState_DodgeDiveEvent)
{
    prepareState(*this);
    tiggerAndExpect<DodgeDiveEvent>({sut_.animationClipsNames_.disarmed.dodgeDive});
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectRotationLeft(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.rotate.left});
}

TEST_F(CharacterControllerTests, DisarmedRotateState_DodgeForwardEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedRotateState_DodgeBackwardEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedRotateState_DodgeLeftEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedRotateState_DodgeRightEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedRotateState_DodgeEndEvent)
{
    prepareState(*this);
    expectIgnoreEvent<DodgeEndEvent>();
}

TEST_F(CharacterControllerTests, DisarmedRotateState_AttackEvent)
{
    prepareState(*this);
    tiggerAndExpect<AttackEvent>({sut_.animationClipsNames_.disarmed.attack.front().clipsSequence.front(),
                                  sut_.animationClipsNames_.disarmed.posture.stand.rotate.left},
                                 {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_TRANSITION_TIME});

    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectRotationLeft(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.rotate.left});
}

TEST_F(CharacterControllerTests, DisarmedRotateState_EndAttackEvent)
{
    prepareState(*this);
    expectIgnoreEvent<EndAttackEvent>();
}

TEST_F(CharacterControllerTests, DisarmedRotateState_EquipEndStateEvent)
{
    prepareState(*this);
    expectIgnoreEvent<EquipEndStateEvent>();
}

TEST_F(CharacterControllerTests, DisarmedRotateState_DisarmEndStateEvent)
{
    prepareState(*this);
    expectIgnoreEvent<DisarmEndStateEvent>();
}

TEST_F(CharacterControllerTests, DisarmedRotateState_ReloadArrowEvent)
{
    prepareState(*this);
    expectRotationLeft(ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<ReloadArrowEvent>(
        {sut_.animationClipsNames_.armed.posture.stand.rotate.left, sut_.animationClipsNames_.equip.front()},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_TRANSITION_TIME});
    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectRotationLeft(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.posture.stand.rotate.left, sut_.animationClipsNames_.aim.draw});
}

TEST_F(CharacterControllerTests, DisarmedRotateState_AimStartEvent)
{
    prepareState(*this);
    tiggerAndExpect<AimStartEvent>(
        {sut_.animationClipsNames_.armed.posture.stand.rotate.left, sut_.animationClipsNames_.equip.front()},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_TRANSITION_TIME});
    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectRotationLeft(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.posture.stand.rotate.left, sut_.animationClipsNames_.aim.draw});
    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectRotationLeft(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.posture.stand.rotate.left, sut_.animationClipsNames_.aim.idle});
}

TEST_F(CharacterControllerTests, DisarmedRotateState_AimStopEvent)
{
    prepareState(*this);
    expectIgnoreEvent<AimStopEvent>();
}

TEST_F(CharacterControllerTests, DisarmedRotateState_CrouchChangeStateEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedRotateState_StartFallingEvent)
{
    prepareState(*this);
    tiggerAndExpect<StartFallingEvent>({sut_.animationClipsNames_.disarmed.falling});
}

TEST_F(CharacterControllerTests, DisarmedRotateState_GroundDetectionEvent)
{
    prepareState(*this);
    expectIgnoreEvent<GroundDetectionEvent>();
}

TEST_F(CharacterControllerTests, DisarmedRotateState_HurtEvent)
{
    prepareState(*this);
    tiggerAndExpect<HurtEvent>({sut_.animationClipsNames_.disarmed.posture.stand.hurt});
}

TEST_F(CharacterControllerTests, DisarmedRotateState_AnimationPlayEndEvent)
{
    prepareState(*this);
    expectIgnoreEvent<AnimationPlayEndEvent>();
}

TEST_F(CharacterControllerTests, DisarmedRotateState_ChangeAnimEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedRotateState_StartDialogEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedRotateState_EndDialogEvent)
{
    prepareState(*this);
    expectIgnoreEvent<EndDialogEvent>();
}
