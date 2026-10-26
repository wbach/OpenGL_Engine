#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    test.expectRotationLeft();
    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.posture.stand.idle});
    test.tiggerAndExpect<RotateLeftEvent>({test.sut_.animationClipsNames_.disarmed.posture.stand.rotate.left});
    test.tiggerAndExpect<WeaponStateEvent>(
        {test.sut_.animationClipsNames_.armed.posture.stand.rotate.left, test.sut_.animationClipsNames_.equip.front()});
}
}  // namespace

TEST_F(CharacterControllerTests, RotateArmedChangeState_WeaponStateEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RotateArmedChangeState_CrouchChangeStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<CrouchChangeStateEvent>(
        {sut_.animationClipsNames_.armed.posture.crouched.rotate.left, sut_.animationClipsNames_.equip.front()});
}
TEST_F(CharacterControllerTests, RotateArmedChangeState_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent>({sut_.animationClipsNames_.disarmed.posture.stand.death});
}
TEST_F(CharacterControllerTests, RotateArmedChangeState_EquipEndStateEvent)
{
    prepareState(*this);
    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    tiggerAndExpect<EquipEndStateEvent>({sut_.animationClipsNames_.armed.posture.stand.rotate.left},
                                        {ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_CLIP_TIME});
}
TEST_F(CharacterControllerTests, RotateArmedChangeState_DisarmEndStateEvent)
{
    prepareState(*this);
    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    tiggerAndExpect<DisarmEndStateEvent>({sut_.animationClipsNames_.disarmed.posture.stand.rotate.left},
                                         {ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_CLIP_TIME});
}
TEST_F(CharacterControllerTests, RotateArmedChangeState_MoveForwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveForwardEvent>(
        {sut_.animationClipsNames_.equip.front(), sut_.animationClipsNames_.armed.movement.run.forward});
}
TEST_F(CharacterControllerTests, RotateArmedChangeState_MoveBackwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(-DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveBackwardEvent>(
        {sut_.animationClipsNames_.equip.front(), sut_.animationClipsNames_.armed.movement.run.forward});
    expectRotationLeft(DEFAULT_MOVING_CHANGE_DIR_SPEED);
    expectRootboneRotation(VECTOR_BACKWARD);
}
TEST_F(CharacterControllerTests, RotateArmedChangeState_MoveLeftEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_LEFT, vec3(DEFAULT_RUN_SPEED, 0.0, 0.0));
    tiggerAndExpect<MoveLeftEvent>(
        {sut_.animationClipsNames_.equip.front(), sut_.animationClipsNames_.armed.movement.run.forward});
    expectRotationLeft(DEFAULT_MOVING_CHANGE_DIR_SPEED);
    expectRootboneRotation(VECTOR_LEFT);
}
TEST_F(CharacterControllerTests, RotateArmedChangeState_MoveRightEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_RIGHT, vec3(DEFAULT_RUN_SPEED, 0.0, 0.0));
    tiggerAndExpect<MoveRightEvent>(
        {sut_.animationClipsNames_.equip.front(), sut_.animationClipsNames_.armed.movement.run.forward});
    expectRotationLeft(DEFAULT_MOVING_CHANGE_DIR_SPEED);
    expectRootboneRotation(VECTOR_RIGHT);
}
TEST_F(CharacterControllerTests, RotateArmedChangeState_RotateLeftEvent)
{
    prepareState(*this);
    expectRotationLeft();
    tiggerAndExpect<RotateLeftEvent>(
        {sut_.animationClipsNames_.equip.front(), sut_.animationClipsNames_.armed.posture.stand.rotate.left});
}
TEST_F(CharacterControllerTests, RotateArmedChangeState_RotateRightEvent)
{
    prepareState(*this);
    expectRotationRight();
    tiggerAndExpect<RotateRightEvent>(
        {sut_.animationClipsNames_.equip.front(), sut_.animationClipsNames_.armed.posture.stand.rotate.right});
}
TEST_F(CharacterControllerTests, RotateArmedChangeState_RotateTargetEvent)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent>(
        RotateTargetEvent{targetRotation.value_},
        {sut_.animationClipsNames_.equip.front(), sut_.animationClipsNames_.armed.posture.stand.rotate.right});
}
TEST_F(CharacterControllerTests, RotateArmedChangeState_DrawArrowEvent)
{
    prepareState(*this);
    tiggerAndExpect<DrawArrowEvent>(
        {sut_.animationClipsNames_.equip.front(), sut_.animationClipsNames_.armed.posture.stand.rotate.left});
    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectRotationLeft(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.aim.draw, sut_.animationClipsNames_.armed.posture.stand.rotate.left});
}
TEST_F(CharacterControllerTests, RotateArmedChangeState_AimStopEvent)
{
    prepareState(*this);
    tiggerAndExpect<DrawArrowEvent>(
        {sut_.animationClipsNames_.equip.front(), sut_.animationClipsNames_.armed.posture.stand.rotate.left});
    tiggerAndExpect<AimStopEvent>(
        {sut_.animationClipsNames_.equip.front(), sut_.animationClipsNames_.armed.posture.stand.rotate.left});
    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectRotationLeft(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.posture.stand.rotate.left});
}
TEST_F(CharacterControllerTests, RotateArmedChangeState_MoveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RotateArmedChangeState_WalkChangeStateEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RotateArmedChangeState_SprintStateChangeEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RotateArmedChangeState_EndForwardMoveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RotateArmedChangeState_EndBackwardMoveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RotateArmedChangeState_EndMoveLeftEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RotateArmedChangeState_EndMoveRightEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RotateArmedChangeState_EndRotationEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RotateArmedChangeState_JumpEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RotateArmedChangeState_JumpConfirmEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RotateArmedChangeState_DodgeDiveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RotateArmedChangeState_DodgeForwardEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RotateArmedChangeState_DodgeBackwardEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RotateArmedChangeState_DodgeLeftEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RotateArmedChangeState_DodgeRightEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RotateArmedChangeState_DodgeEndEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RotateArmedChangeState_AttackEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RotateArmedChangeState_EndAttackEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RotateArmedChangeState_ReloadArrowEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RotateArmedChangeState_AimStartEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RotateArmedChangeState_StartFallingEvent)
{
    prepareState(*this);
    tiggerAndExpect<StartFallingEvent>({sut_.animationClipsNames_.armed.falling});
}
TEST_F(CharacterControllerTests, RotateArmedChangeState_GroundDetectionEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RotateArmedChangeState_HurtEvent)
{
    prepareState(*this);
    tiggerAndExpect<HurtEvent>({sut_.animationClipsNames_.disarmed.posture.stand.hurt});
}
TEST_F(CharacterControllerTests, RotateArmedChangeState_AnimationPlayEndEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RotateArmedChangeState_ChangeAnimEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RotateArmedChangeState_StartDialogEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RotateArmedChangeState_EndDialogEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
