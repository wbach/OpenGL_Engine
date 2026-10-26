#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    EXPECT_CALL(test.inputManagerMock_, CalcualteMouseMove()).WillRepeatedly(Return(vec2i{0, 0}));
    EXPECT_CALL(test.physicsApiMock_, GetVelocity(test.rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(test.physicsApiMock_, GetRotation(test.rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    test.expectForwardVelocity(DEFAULT_WALK_SPEED);

    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.posture.stand.idle});
    test.tiggerAndExpect<WeaponStateEvent>(
        {test.sut_.animationClipsNames_.armed.posture.stand.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    test.tiggerAndExpect<DrawArrowEvent>({test.sut_.animationClipsNames_.aim.draw});
    test.Update(ADVANCED_TIME_CLIP_TIME);
    test.Update(ADVANCED_TIME_TRANSITION_TIME);

    test.tiggerAndExpect<AttackEvent>({test.sut_.animationClipsNames_.aim.recoil});
    test.tiggerAndExpect<MoveForwardEvent>(
        {test.sut_.animationClipsNames_.aim.recoil, test.sut_.animationClipsNames_.armed.movement.walk.forward});
    test.tiggerAndExpect<CrouchChangeStateEvent>(
        {test.sut_.animationClipsNames_.aim.recoil, test.sut_.animationClipsNames_.armed.movement.crouch.forward});
}
}  // namespace

TEST_F(CharacterControllerTests, RecoilCrouchWalk_RotateLeft)
{
    prepareState(*this);
    expectAnyRotation();
    tiggerAndExpect<RotateLeftEvent>(
        {sut_.animationClipsNames_.aim.recoil, sut_.animationClipsNames_.armed.movement.crouch.forward});
}
TEST_F(CharacterControllerTests, RecoilCrouchWalk_RotateRight)
{
    prepareState(*this);
    expectAnyRotation();
    tiggerAndExpect<RotateRightEvent>(
        {sut_.animationClipsNames_.aim.recoil, sut_.animationClipsNames_.armed.movement.crouch.forward});
}
TEST_F(CharacterControllerTests, RecoilCrouchWalk_RotateTargetEvent)
{
    prepareState(*this);
    expectAnyRotation();

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent>(
        RotateTargetEvent{targetRotation.value_},
        {sut_.animationClipsNames_.aim.recoil, sut_.animationClipsNames_.armed.movement.crouch.forward});
}
TEST_F(CharacterControllerTests, RecoilCrouchWalk_CrouchChangeStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<CrouchChangeStateEvent>(
        {sut_.animationClipsNames_.aim.recoil, sut_.animationClipsNames_.armed.movement.walk.forward});
}
TEST_F(CharacterControllerTests, RecoilCrouchWalk_WeaponStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<WeaponStateEvent>(
        {sut_.animationClipsNames_.disarm.front(), sut_.animationClipsNames_.disarmed.movement.crouch.forward});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.movement.crouch.forward});
}
TEST_F(CharacterControllerTests, RecoilCrouchWalk_ReloadArrowEvent)
{
    prepareState(*this);
    tiggerAndExpect<ReloadArrowEvent>(
        {sut_.animationClipsNames_.aim.draw, sut_.animationClipsNames_.armed.movement.crouch.forward});
}
TEST_F(CharacterControllerTests, RecoilCrouchWalk_AimStopEvent)
{
    prepareState(*this);
    tiggerAndExpect<AimStopEvent>({sut_.animationClipsNames_.armed.movement.crouch.forward});
}
TEST_F(CharacterControllerTests, RecoilCrouchWalk_MoveForwardEvent)
{
    prepareState(*this);
    tiggerAndExpect<MoveForwardEvent>(
        {sut_.animationClipsNames_.aim.recoil, sut_.animationClipsNames_.armed.movement.crouch.forward});
}
TEST_F(CharacterControllerTests, RecoilCrouchWalk_MoveBackwardEvent)
{
    prepareState(*this);
    expectNoMove();
    tiggerAndExpect<MoveBackwardEvent>(
        {sut_.animationClipsNames_.aim.recoil, sut_.animationClipsNames_.armed.posture.crouched.idle});
}
TEST_F(CharacterControllerTests, RecoilCrouchWalk_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent>({sut_.animationClipsNames_.armed.posture.crouched.death});
}
TEST_F(CharacterControllerTests, RecoilCrouchWalk_SprintStateChangeEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent>({sut_.animationClipsNames_.armed.sprint});
}
TEST_F(CharacterControllerTests, RecoilCrouchWalk_WalkChangeStateEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent>(
        {sut_.animationClipsNames_.aim.recoil, sut_.animationClipsNames_.armed.movement.crouch.forward});
}
TEST_F(CharacterControllerTests, RecoilCrouchWalk_MoveLeftEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_FORWARD + VECTOR_LEFT, vec3(DEFAULT_WALK_LEFT_RIGHT_SPEED, 0.0, DEFAULT_WALK_SPEED));
    tiggerAndExpect<MoveLeftEvent>(
        {sut_.animationClipsNames_.aim.recoil, sut_.animationClipsNames_.armed.movement.crouch.forward});
}
TEST_F(CharacterControllerTests, RecoilCrouchWalk_RunBackwardEvent)
{
    prepareState(*this);
    expectNoMove();
    tiggerAndExpect<MoveBackwardEvent>(
        {sut_.animationClipsNames_.aim.recoil, sut_.animationClipsNames_.armed.posture.crouched.idle});
    expectVelocity(VECTOR_BACKWARD, vec3(0.0, 0.0, DEFAULT_BACKWARD_WALK_SPEED));
    tiggerAndExpect<EndForwardMoveEvent>(
        {sut_.animationClipsNames_.aim.recoil, sut_.animationClipsNames_.armed.movement.crouch.backward});
}
TEST_F(CharacterControllerTests, RecoilCrouchWalk_EndForwardMoveEvent)
{
    prepareState(*this);
    tiggerAndExpect<EndForwardMoveEvent>(
        {sut_.animationClipsNames_.aim.recoil, sut_.animationClipsNames_.armed.posture.crouched.idle});
}
TEST_F(CharacterControllerTests, RecoilCrouchWalk_EndBackwardMoveEvent)
{
    prepareState(*this);
    tiggerAndExpect<EndForwardMoveEvent>(
        {sut_.animationClipsNames_.aim.recoil, sut_.animationClipsNames_.armed.posture.crouched.idle});
    expectForwardVelocity(-DEFAULT_BACKWARD_WALK_SPEED);
    tiggerAndExpect<MoveBackwardEvent>(
        {sut_.animationClipsNames_.aim.recoil, sut_.animationClipsNames_.armed.movement.crouch.backward});
    tiggerAndExpect<EndBackwardMoveEvent>(
        {sut_.animationClipsNames_.aim.recoil, sut_.animationClipsNames_.armed.posture.crouched.idle}, {0});
}
TEST_F(CharacterControllerTests, RecoilCrouchWalk_ReloadArrowEventWhenRunLeftAndRight)
{
    prepareState(*this);
    tiggerAndExpect<EndForwardMoveEvent>(
        {sut_.animationClipsNames_.aim.recoil, sut_.animationClipsNames_.armed.posture.crouched.idle});
    expectVelocity(VECTOR_LEFT, vec3(DEFAULT_WALK_LEFT_RIGHT_SPEED, 0.f, 0.f));
    tiggerAndExpect<MoveLeftEvent>({sut_.animationClipsNames_.aim.recoil, sut_.animationClipsNames_.armed.movement.crouch.left});
    expectNoMove();
    tiggerAndExpect<MoveRightEvent>({sut_.animationClipsNames_.aim.recoil, sut_.animationClipsNames_.armed.posture.crouched.idle},
                                    {0});
    expectNoMove();
    tiggerAndExpect<ReloadArrowEvent>({sut_.animationClipsNames_.aim.draw, sut_.animationClipsNames_.armed.posture.crouched.idle},
                                      {0});
}
TEST_F(CharacterControllerTests, RecoilCrouchWalk_MoveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilCrouchWalk_MoveRightEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilCrouchWalk_EndMoveLeftEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilCrouchWalk_EndMoveRightEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilCrouchWalk_EndRotationEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilCrouchWalk_JumpEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilCrouchWalk_JumpConfirmEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilCrouchWalk_DodgeDiveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilCrouchWalk_DodgeForwardEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilCrouchWalk_DodgeBackwardEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilCrouchWalk_DodgeLeftEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilCrouchWalk_DodgeRightEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilCrouchWalk_DodgeEndEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilCrouchWalk_EndAttackEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilCrouchWalk_EquipEndStateEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilCrouchWalk_DisarmEndStateEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilCrouchWalk_AimStartEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilCrouchWalk_StartFallingEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilCrouchWalk_GroundDetectionEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilCrouchWalk_HurtEvent)
{
    prepareState(*this);
    tiggerAndExpect<HurtEvent>({sut_.animationClipsNames_.armed.posture.crouched.hurt});
}
TEST_F(CharacterControllerTests, RecoilCrouchWalk_AnimationPlayEndEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilCrouchWalk_ChangeAnimEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilCrouchWalk_StartDialogEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilCrouchWalk_EndDialogEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilCrouchWalk_AttackEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilCrouchWalk_DrawArrowEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilCrouchWalk_RotateLeftEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilCrouchWalk_RotateRightEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
