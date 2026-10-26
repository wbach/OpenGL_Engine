#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    EXPECT_CALL(test.inputManagerMock_, CalcualteMouseMove()).WillRepeatedly(Return(vec2i{0, 0}));
    EXPECT_CALL(test.physicsApiMock_, GetRotation(test.rigidbodyid)).WillRepeatedly(Return(Rotation().value_));

    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.posture.stand.idle});
    test.tiggerAndExpect<WeaponStateEvent>(
        {test.sut_.animationClipsNames_.armed.posture.stand.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    test.tiggerAndExpect<CrouchChangeStateEvent>({test.sut_.animationClipsNames_.armed.posture.crouched.idle});
    test.tiggerAndExpect<DrawArrowEvent>(
        {test.sut_.animationClipsNames_.aim.draw, test.sut_.animationClipsNames_.armed.posture.crouched.idle});
    test.Update(ADVANCED_TIME_CLIP_TIME);
    test.Update(ADVANCED_TIME_TRANSITION_TIME);

    test.tiggerAndExpect<AttackEvent>(
        {test.sut_.animationClipsNames_.aim.recoil, test.sut_.animationClipsNames_.armed.posture.crouched.idle});
}
}  // namespace

TEST_F(CharacterControllerTests, RecoilCrouchState_CrouchChangeStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<CrouchChangeStateEvent>({sut_.animationClipsNames_.aim.recoil});
}
TEST_F(CharacterControllerTests, RecoilCrouchState_RotateLeft)
{
    prepareState(*this);
    expectAnyRotation();
    tiggerAndExpect<RotateLeftEvent>(
        {sut_.animationClipsNames_.aim.recoil, sut_.animationClipsNames_.armed.posture.crouched.rotate.left});
}
TEST_F(CharacterControllerTests, RecoilCrouchState_RotateRight)
{
    prepareState(*this);
    expectAnyRotation();
    tiggerAndExpect<RotateRightEvent>(
        {sut_.animationClipsNames_.aim.recoil, sut_.animationClipsNames_.armed.posture.crouched.rotate.right});
}
TEST_F(CharacterControllerTests, RecoilCrouchState_RotateTargetEvent)
{
    prepareState(*this);
    expectAnyRotation();

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent>(
        RotateTargetEvent{targetRotation.value_},
        {sut_.animationClipsNames_.aim.recoil, sut_.animationClipsNames_.armed.posture.crouched.rotate.right});
}
TEST_F(CharacterControllerTests, RecoilCrouchState_WeaponStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<WeaponStateEvent>(
        {sut_.animationClipsNames_.disarm.front(), sut_.animationClipsNames_.armed.posture.crouched.idle});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.crouched.idle});
}
TEST_F(CharacterControllerTests, RecoilCrouchState_AttackEvent)
{
    prepareState(*this);
    tiggerAndExpect<AttackEvent>({sut_.animationClipsNames_.aim.recoil, sut_.animationClipsNames_.armed.posture.crouched.idle});
}
TEST_F(CharacterControllerTests, RecoilCrouchState_AimStopEvent)
{
    prepareState(*this);
    tiggerAndExpect<AimStopEvent>({sut_.animationClipsNames_.armed.posture.crouched.idle});
}
TEST_F(CharacterControllerTests, RecoilCrouchState_ReloadArrowEvent)
{
    prepareState(*this);
    tiggerAndExpect<ReloadArrowEvent>(
        {sut_.animationClipsNames_.aim.draw, sut_.animationClipsNames_.armed.posture.crouched.idle});
}
TEST_F(CharacterControllerTests, RecoilCrouchState_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent>({sut_.animationClipsNames_.armed.posture.crouched.death});
}
TEST_F(CharacterControllerTests, RecoilCrouchState_MoveForwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<MoveForwardEvent>(
        {sut_.animationClipsNames_.armed.movement.crouch.forward, sut_.animationClipsNames_.aim.recoil});
}
TEST_F(CharacterControllerTests, RecoilCrouchState_RunBackwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(-DEFAULT_BACKWARD_WALK_SPEED);
    tiggerAndExpect<MoveBackwardEvent>(
        {sut_.animationClipsNames_.armed.movement.crouch.backward, sut_.animationClipsNames_.aim.recoil});
}
TEST_F(CharacterControllerTests, RecoilCrouchState_WalkForwardEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_LEFT, vec3(DEFAULT_WALK_LEFT_RIGHT_SPEED, 0.0, 0));
    tiggerAndExpect<MoveLeftEvent>({sut_.animationClipsNames_.armed.movement.crouch.left, sut_.animationClipsNames_.aim.recoil});
}
TEST_F(CharacterControllerTests, RecoilCrouchState_MoveRightEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_RIGHT, vec3(DEFAULT_WALK_LEFT_RIGHT_SPEED, 0.0, 0));
    tiggerAndExpect<MoveRightEvent>(
        {sut_.animationClipsNames_.armed.movement.crouch.right, sut_.animationClipsNames_.aim.recoil});
}

TEST_F(CharacterControllerTests, RecoilCrouchState_MoveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilCrouchState_MoveBackwardEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilCrouchState_MoveLeftEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilCrouchState_WalkChangeStateEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilCrouchState_SprintStateChangeEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilCrouchState_EndForwardMoveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilCrouchState_EndBackwardMoveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilCrouchState_EndMoveLeftEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilCrouchState_EndMoveRightEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilCrouchState_RotateLeftEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilCrouchState_RotateRightEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilCrouchState_EndRotationEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilCrouchState_JumpEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilCrouchState_JumpConfirmEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilCrouchState_DodgeDiveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilCrouchState_DodgeForwardEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilCrouchState_DodgeBackwardEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilCrouchState_DodgeLeftEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilCrouchState_DodgeRightEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilCrouchState_DodgeEndEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilCrouchState_EndAttackEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilCrouchState_EquipEndStateEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilCrouchState_DisarmEndStateEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilCrouchState_DrawArrowEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilCrouchState_AimStartEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilCrouchState_StartFallingEvent)
{
    prepareState(*this);
    tiggerAndExpect<StartFallingEvent>({sut_.animationClipsNames_.armed.falling});
}
TEST_F(CharacterControllerTests, RecoilCrouchState_GroundDetectionEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilCrouchState_HurtEvent)
{
    prepareState(*this);
    tiggerAndExpect<HurtEvent>({sut_.animationClipsNames_.armed.posture.crouched.hurt});
}
TEST_F(CharacterControllerTests, RecoilCrouchState_AnimationPlayEndEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilCrouchState_ChangeAnimEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilCrouchState_StartDialogEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilCrouchState_EndDialogEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
