#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    EXPECT_CALL(test.physicsApiMock_, GetVelocity(test.rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(test.physicsApiMock_, GetRotation(test.rigidbodyid)).WillRepeatedly(Return(Rotation().value_));

    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.posture.stand.idle});
    test.triggerAndExpect<WeaponStateEvent>(
        {test.sut_.animationClipsNames_.armed.posture.stand.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    test.expectRotationRight(ADVANCED_TIME_TRANSITION_TIME);
    test.triggerAndExpect<RotateRightEvent>({test.sut_.animationClipsNames_.armed.posture.stand.rotate.right});
}
}  // namespace

TEST_F(CharacterControllerTests, ArmedRotate_CrouchChangeStateEvent)
{
    prepareState(*this);
    triggerAndExpect<CrouchChangeStateEvent>({sut_.animationClipsNames_.armed.posture.crouched.rotate.right});
}

TEST_F(CharacterControllerTests, ArmedRotate_DrawArrowEvent)
{
    prepareState(*this);
    triggerAndExpect<DrawArrowEvent>(
        {sut_.animationClipsNames_.aim.draw, sut_.animationClipsNames_.armed.posture.stand.rotate.right});
}

TEST_F(CharacterControllerTests, ArmedRotate_DeathEvent)
{
    prepareState(*this);
    triggerAndExpect<DeathEvent>({sut_.animationClipsNames_.armed.posture.stand.death});
}

TEST_F(CharacterControllerTests, ArmedRotate_WeaponStateEvent)
{
    prepareState(*this);
    triggerAndExpect<WeaponStateEvent>(
        {sut_.animationClipsNames_.disarmed.posture.stand.rotate.right, sut_.animationClipsNames_.disarm.front()});
}

TEST_F(CharacterControllerTests, ArmedRotate_MoveForwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    triggerAndExpect<MoveForwardEvent>({sut_.animationClipsNames_.armed.movement.run.forward});
}

TEST_F(CharacterControllerTests, ArmedRotate_MoveBackwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(-DEFAULT_RUN_SPEED);
    triggerAndExpect<MoveBackwardEvent>({sut_.animationClipsNames_.armed.movement.run.forward});
    expectRotationRight(DEFAULT_MOVING_CHANGE_DIR_SPEED);
    expectRootboneRotation(VECTOR_BACKWARD);
}

TEST_F(CharacterControllerTests, ArmedRotate_MoveLeftEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_LEFT, vec3(DEFAULT_RUN_SPEED, 0.0, 0));
    triggerAndExpect<MoveLeftEvent>({sut_.animationClipsNames_.armed.movement.run.forward});
    expectRotationRight(DEFAULT_MOVING_CHANGE_DIR_SPEED);
    expectRootboneRotation(VECTOR_LEFT);
}

TEST_F(CharacterControllerTests, ArmedRotate_MoveRightEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_RIGHT, vec3(DEFAULT_RUN_SPEED, 0.0, 0));
    triggerAndExpect<MoveRightEvent>({sut_.animationClipsNames_.armed.movement.run.forward});
    expectRotationRight(DEFAULT_MOVING_CHANGE_DIR_SPEED);
    expectRootboneRotation(VECTOR_RIGHT);
}

TEST_F(CharacterControllerTests, ArmedRotate_RotateLeftEvent)
{
    prepareState(*this);
    expectRotationRight();
    triggerAndExpect<RotateRightEvent>({sut_.animationClipsNames_.armed.posture.stand.rotate.right});
    expectRotationLeft();
    triggerAndExpect<RotateLeftEvent>({sut_.animationClipsNames_.armed.posture.stand.rotate.left});
}

TEST_F(CharacterControllerTests, ArmedRotate_RotateTargetEvent)
{
    prepareState(*this);
    expectRotationRight();
    triggerAndExpect<RotateRightEvent>({sut_.animationClipsNames_.armed.posture.stand.rotate.right});

    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    triggerAndExpect<RotateTargetEvent>(RotateTargetEvent{targetRotation.value_},
                                       {sut_.animationClipsNames_.armed.posture.stand.rotate.right});
}

TEST_F(CharacterControllerTests, ArmedRotate_EndRotationEvent)
{
    prepareState(*this);
    triggerAndExpect<EndRotationEvent>({sut_.animationClipsNames_.armed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, ArmedRotate_MoveEvent)
{
    prepareState(*this);
    // TODO: Implement test for MoveEvent
}

TEST_F(CharacterControllerTests, ArmedRotate_WalkChangeStateEvent)
{
    prepareState(*this);
    // TODO: Implement test for WalkChangeStateEvent
}

TEST_F(CharacterControllerTests, ArmedRotate_SprintStateChangeEvent)
{
    prepareState(*this);
    // TODO: Implement test for SprintStateChangeEvent
}

TEST_F(CharacterControllerTests, ArmedRotate_EndForwardMoveEvent)
{
    prepareState(*this);
    // TODO: Implement test for EndForwardMoveEvent
}

TEST_F(CharacterControllerTests, ArmedRotate_EndBackwardMoveEvent)
{
    prepareState(*this);
    // TODO: Implement test for EndBackwardMoveEvent
}

TEST_F(CharacterControllerTests, ArmedRotate_EndMoveLeftEvent)
{
    prepareState(*this);
    // TODO: Implement test for EndMoveLeftEvent
}

TEST_F(CharacterControllerTests, ArmedRotate_EndMoveRightEvent)
{
    prepareState(*this);
    // TODO: Implement test for EndMoveRightEvent
}

TEST_F(CharacterControllerTests, ArmedRotate_RotateRightEvent)
{
    prepareState(*this);
    // TODO: Implement test for RotateRightEvent
}

TEST_F(CharacterControllerTests, ArmedRotate_JumpEvent)
{
    prepareState(*this);
    // TODO: Implement test for JumpEvent
}

TEST_F(CharacterControllerTests, ArmedRotate_JumpConfirmEvent)
{
    prepareState(*this);
    // TODO: Implement test for JumpConfirmEvent
}

TEST_F(CharacterControllerTests, ArmedRotate_DodgeDiveEvent)
{
    prepareState(*this);
    // TODO: Implement test for DodgeDiveEvent
}

TEST_F(CharacterControllerTests, ArmedRotate_DodgeForwardEvent)
{
    prepareState(*this);
    // TODO: Implement test for DodgeForwardEvent
}

TEST_F(CharacterControllerTests, ArmedRotate_DodgeBackwardEvent)
{
    prepareState(*this);
    // TODO: Implement test for DodgeBackwardEvent
}

TEST_F(CharacterControllerTests, ArmedRotate_DodgeLeftEvent)
{
    prepareState(*this);
    // TODO: Implement test for DodgeLeftEvent
}

TEST_F(CharacterControllerTests, ArmedRotate_DodgeRightEvent)
{
    prepareState(*this);
    // TODO: Implement test for DodgeRightEvent
}

TEST_F(CharacterControllerTests, ArmedRotate_DodgeEndEvent)
{
    prepareState(*this);
    // TODO: Implement test for DodgeEndEvent
}

TEST_F(CharacterControllerTests, ArmedRotate_AttackEvent)
{
    prepareState(*this);
    // TODO: Implement test for AttackEvent
}

TEST_F(CharacterControllerTests, ArmedRotate_EndAttackEvent)
{
    prepareState(*this);
    // TODO: Implement test for EndAttackEvent
}

TEST_F(CharacterControllerTests, ArmedRotate_EquipEndStateEvent)
{
    prepareState(*this);
    // TODO: Implement test for EquipEndStateEvent
}

TEST_F(CharacterControllerTests, ArmedRotate_DisarmEndStateEvent)
{
    prepareState(*this);
    // TODO: Implement test for DisarmEndStateEvent
}

TEST_F(CharacterControllerTests, ArmedRotate_ReloadArrowEvent)
{
    prepareState(*this);
    // TODO: Implement test for ReloadArrowEvent
}

TEST_F(CharacterControllerTests, ArmedRotate_AimStartEvent)
{
    prepareState(*this);
    // TODO: Implement test for AimStartEvent
}

TEST_F(CharacterControllerTests, ArmedRotate_AimStopEvent)
{
    prepareState(*this);
    // TODO: Implement test for AimStopEvent
}

TEST_F(CharacterControllerTests, ArmedRotate_StartFallingEvent)
{
    prepareState(*this);
    triggerAndExpect<StartFallingEvent>({sut_.animationClipsNames_.armed.falling});
}

TEST_F(CharacterControllerTests, ArmedRotate_GroundDetectionEvent)
{
    prepareState(*this);
    // TODO: Implement test for GroundDetectionEvent
}

TEST_F(CharacterControllerTests, ArmedRotate_HurtEvent)
{
    prepareState(*this);
    triggerAndExpect<HurtEvent>({sut_.animationClipsNames_.armed.posture.stand.hurt});
}

TEST_F(CharacterControllerTests, ArmedRotate_AnimationPlayEndEvent)
{
    prepareState(*this);
    // TODO: Implement test for AnimationPlayEndEvent
}

TEST_F(CharacterControllerTests, ArmedRotate_ChangeAnimEvent)
{
    prepareState(*this);
    // TODO: Implement test for ChangeAnimEvent
}

TEST_F(CharacterControllerTests, ArmedRotate_StartDialogEvent)
{
    prepareState(*this);

    expectNoMove();
    expectNoRotation();

    triggerAndExpect({sut_.animationClipsNames_.disarm.front()}, {ADVANCED_TIME_TRANSITION_TIME},
                    StartDialogEvent{.role = StartDialogEvent::Role::Listener});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.dialog.listen});
}

TEST_F(CharacterControllerTests, ArmedRotate_EndDialogEvent)
{
    prepareState(*this);
    // TODO: Implement test for EndDialogEvent
}
