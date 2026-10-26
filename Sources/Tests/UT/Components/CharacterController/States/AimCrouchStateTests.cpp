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
}
}  // namespace

TEST_F(CharacterControllerTests, AimCrouchState_CrouchChangeStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<CrouchChangeStateEvent>({sut_.animationClipsNames_.aim.idle});
}

TEST_F(CharacterControllerTests, AimCrouchState_RotateLeftEvent)
{
    prepareState(*this);
    expectRotationLeft();
    tiggerAndExpect<RotateLeftEvent>(
        {sut_.animationClipsNames_.aim.idle, sut_.animationClipsNames_.armed.posture.crouched.rotate.left});
}
TEST_F(CharacterControllerTests, AimCrouchState_RotateRightEvent)
{
    prepareState(*this);
    expectRotationRight();
    tiggerAndExpect<RotateRightEvent>(
        {sut_.animationClipsNames_.aim.idle, sut_.animationClipsNames_.armed.posture.crouched.rotate.right});
}
TEST_F(CharacterControllerTests, AimCrouchState_RotateTargetEvent)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent>(
        RotateTargetEvent{targetRotation.value_},
        {sut_.animationClipsNames_.aim.idle, sut_.animationClipsNames_.armed.posture.crouched.rotate.right});
}
TEST_F(CharacterControllerTests, AimCrouchState_WeaponStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<WeaponStateEvent>(
        {sut_.animationClipsNames_.disarm.front(), sut_.animationClipsNames_.armed.posture.crouched.idle});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.crouched.idle});
}
TEST_F(CharacterControllerTests, AimCrouchState_AttackEvent)
{
    prepareState(*this);
    tiggerAndExpect<AttackEvent>({sut_.animationClipsNames_.aim.recoil, sut_.animationClipsNames_.armed.posture.crouched.idle});
}
TEST_F(CharacterControllerTests, AimCrouchState_AimStopEvent)
{
    prepareState(*this);
    tiggerAndExpect<AimStopEvent>({sut_.animationClipsNames_.armed.posture.crouched.idle});
}
TEST_F(CharacterControllerTests, AimCrouchState_MoveForwardEvent)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<MoveForwardEvent>(
        {sut_.animationClipsNames_.aim.idle, sut_.animationClipsNames_.armed.movement.crouch.forward});
}
TEST_F(CharacterControllerTests, AimCrouchState_MoveBackwardEvent)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    expectForwardVelocity(-DEFAULT_BACKWARD_WALK_SPEED);
    tiggerAndExpect<MoveBackwardEvent>(
        {sut_.animationClipsNames_.aim.idle, sut_.animationClipsNames_.armed.movement.crouch.backward});
}
TEST_F(CharacterControllerTests, AimCrouchState_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent>({sut_.animationClipsNames_.armed.posture.crouched.death});
}
TEST_F(CharacterControllerTests, AimCrouchState_MoveLeftEvent)
{
    prepareState(*this);
    expectLeftVelocity(DEFAULT_WALK_LEFT_RIGHT_SPEED);
    tiggerAndExpect<MoveLeftEvent>({sut_.animationClipsNames_.armed.movement.crouch.left, sut_.animationClipsNames_.aim.idle});
}
TEST_F(CharacterControllerTests, AimCrouchState_MoveRightEvent)
{
    prepareState(*this);
    expectLeftVelocity(-DEFAULT_WALK_LEFT_RIGHT_SPEED);
    tiggerAndExpect<MoveRightEvent>({sut_.animationClipsNames_.armed.movement.crouch.right, sut_.animationClipsNames_.aim.idle});
}

TEST_F(CharacterControllerTests, AimCrouchState_AnimationPlayEndEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, AimCrouchState_MoveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, AimCrouchState_WalkChangeStateEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, AimCrouchState_SprintStateChangeEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, AimCrouchState_EndForwardMoveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, AimCrouchState_EndBackwardMoveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, AimCrouchState_EndMoveLeftEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, AimCrouchState_EndMoveRightEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, AimCrouchState_EndRotationEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, AimCrouchState_JumpEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, AimCrouchState_JumpConfirmEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, AimCrouchState_DodgeDiveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, AimCrouchState_DodgeForwardEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, AimCrouchState_DodgeBackwardEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, AimCrouchState_DodgeLeftEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, AimCrouchState_DodgeRightEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, AimCrouchState_DodgeEndEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, AimCrouchState_EndAttackEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, AimCrouchState_EquipEndStateEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, AimCrouchState_DisarmEndStateEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, AimCrouchState_ReloadArrowEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, AimCrouchState_AimStartEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, AimCrouchState_StartFallingEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, AimCrouchState_GroundDetectionEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, AimCrouchState_HurtEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, AimCrouchState_ChangeAnimEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, AimCrouchState_StartDialogEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, AimCrouchState_EndDialogEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, AimCrouchState_DrawArrowEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
