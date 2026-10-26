#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    EXPECT_CALL(test.inputManagerMock_, CalcualteMouseMove()).WillRepeatedly(Return(vec2i{0, 0}));
    // EXPECT_CALL(test.physicsApiMock_, SetRotation(test.rigidbodyid, Matcher<const
    // Quaternion&>(_))).Times(AtLeast(1));
    EXPECT_CALL(test.physicsApiMock_, GetRotation(test.rigidbodyid)).WillRepeatedly(Return(Rotation().value_));

    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.posture.stand.idle});
    test.tiggerAndExpect<WeaponStateEvent>(
        {test.sut_.animationClipsNames_.armed.posture.stand.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    test.tiggerAndExpect<DrawArrowEvent>({test.sut_.animationClipsNames_.aim.draw});
    test.Update(ADVANCED_TIME_CLIP_TIME);
    test.Update(ADVANCED_TIME_TRANSITION_TIME);

    test.expectRotationLeft(ADVANCED_TIME_TRANSITION_TIME);
    test.tiggerAndExpect<RotateLeftEvent>(
        {test.sut_.animationClipsNames_.aim.idle, test.sut_.animationClipsNames_.armed.posture.stand.rotate.left});
    test.tiggerAndExpect<CrouchChangeStateEvent>(
        {test.sut_.animationClipsNames_.aim.idle, test.sut_.animationClipsNames_.armed.posture.crouched.rotate.left});
}
}  // namespace

TEST_F(CharacterControllerTests, AimCrouchRotateState_CrouchChangeStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<CrouchChangeStateEvent>(
        {sut_.animationClipsNames_.aim.idle, sut_.animationClipsNames_.armed.posture.stand.rotate.left});
}
TEST_F(CharacterControllerTests, AimCrouchRotateState_RotateLeftEvent)
{
    prepareState(*this);
    tiggerAndExpect<RotateLeftEvent>(
        {sut_.animationClipsNames_.aim.idle, sut_.animationClipsNames_.armed.posture.crouched.rotate.left});
}
TEST_F(CharacterControllerTests, AimCrouchRotateState_RotateRightEvent)
{
    prepareState(*this);
    expectRotationRight(ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateRightEvent>(
        {sut_.animationClipsNames_.aim.idle, sut_.animationClipsNames_.armed.posture.crouched.rotate.right});
}
TEST_F(CharacterControllerTests, AimCrouchRotateState_RotateTargetEvent)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent>(
        RotateTargetEvent{targetRotation.value_},
        {sut_.animationClipsNames_.aim.idle, sut_.animationClipsNames_.armed.posture.crouched.rotate.right});
}
TEST_F(CharacterControllerTests, AimCrouchRotateState_WeaponStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<WeaponStateEvent>(
        {sut_.animationClipsNames_.disarm.front(), sut_.animationClipsNames_.disarmed.posture.crouched.rotate.left});

    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectRotationLeft(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.crouched.rotate.left});
}
TEST_F(CharacterControllerTests, AimCrouchRotateState_AttackEvent)
{
    prepareState(*this);
    tiggerAndExpect<AttackEvent>(
        {sut_.animationClipsNames_.armed.posture.crouched.rotate.left, sut_.animationClipsNames_.aim.recoil});
}
TEST_F(CharacterControllerTests, AimCrouchRotateState_EndRotationEvent)
{
    prepareState(*this);
    tiggerAndExpect<EndRotationEvent>(
        {sut_.animationClipsNames_.aim.idle, sut_.animationClipsNames_.armed.posture.crouched.idle});
}
TEST_F(CharacterControllerTests, AimCrouchRotateState_AimStopEvent)
{
    prepareState(*this);
    tiggerAndExpect<AimStopEvent>({sut_.animationClipsNames_.armed.posture.crouched.rotate.left});
}
TEST_F(CharacterControllerTests, AimCrouchRotateState_MoveForwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<MoveForwardEvent>(
        {sut_.animationClipsNames_.armed.movement.crouch.forward, sut_.animationClipsNames_.aim.idle});
}
TEST_F(CharacterControllerTests, AimCrouchRotateState_MoveBackwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(-DEFAULT_BACKWARD_WALK_SPEED);
    tiggerAndExpect<MoveBackwardEvent>(
        {sut_.animationClipsNames_.armed.movement.crouch.backward, sut_.animationClipsNames_.aim.idle});
}
TEST_F(CharacterControllerTests, AimCrouchRotateState_MoveLeftEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_LEFT, vec3(DEFAULT_WALK_LEFT_RIGHT_SPEED, 0.f, 0.f));
    tiggerAndExpect<MoveLeftEvent>({sut_.animationClipsNames_.armed.movement.crouch.left, sut_.animationClipsNames_.aim.idle});
}
TEST_F(CharacterControllerTests, AimCrouchRotateState_MoveRightEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_RIGHT, vec3(DEFAULT_WALK_LEFT_RIGHT_SPEED, 0.f, 0.f));
    tiggerAndExpect<MoveRightEvent>({sut_.animationClipsNames_.armed.movement.crouch.right, sut_.animationClipsNames_.aim.idle});
}
TEST_F(CharacterControllerTests, AimCrouchRotateState_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent>({sut_.animationClipsNames_.armed.posture.crouched.death});
}

TEST_F(CharacterControllerTests, AimCrouchRotateState_MoveEvent)
{
    prepareState(*this);

    expectForwardVelocity(DEFAULT_WALK_SPEED);
    // Trigger the move event
    tiggerAndExpect<MoveEvent>({sut_.animationClipsNames_.armed.movement.crouch.forward, sut_.animationClipsNames_.aim.idle});
}

TEST_F(CharacterControllerTests, AimCrouchRotateState_WalkChangeStateEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, AimCrouchRotateState_SprintStateChangeEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, AimCrouchRotateState_EndForwardMoveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, AimCrouchRotateState_EndBackwardMoveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, AimCrouchRotateState_EndMoveLeftEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, AimCrouchRotateState_EndMoveRightEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, AimCrouchRotateState_JumpEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, AimCrouchRotateState_JumpConfirmEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, AimCrouchRotateState_DodgeDiveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, AimCrouchRotateState_DodgeForwardEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, AimCrouchRotateState_DodgeBackwardEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, AimCrouchRotateState_DodgeLeftEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, AimCrouchRotateState_DodgeRightEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, AimCrouchRotateState_DodgeEndEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, AimCrouchRotateState_EndAttackEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, AimCrouchRotateState_EquipEndStateEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, AimCrouchRotateState_DisarmEndStateEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, AimCrouchRotateState_DrawArrowEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, AimCrouchRotateState_ReloadArrowEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, AimCrouchRotateState_AimStartEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, AimCrouchRotateState_StartFallingEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, AimCrouchRotateState_GroundDetectionEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, AimCrouchRotateState_HurtEvent)
{
    prepareState(*this);
    //tiggerAndExpect<HurtEvent>({sut_.animationClipsNames_.armed.posture.crouched.hurt}); TO DO implemnt HURT STATE
}

TEST_F(CharacterControllerTests, AimCrouchRotateState_AnimationPlayEndEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, AimCrouchRotateState_ChangeAnimEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, AimCrouchRotateState_StartDialogEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, AimCrouchRotateState_EndDialogEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
