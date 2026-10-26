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

    test.tiggerAndExpect<DrawArrowEvent>({test.sut_.animationClipsNames_.aim.draw});
    test.Update(ADVANCED_TIME_CLIP_TIME);
    test.Update(ADVANCED_TIME_TRANSITION_TIME);

    test.tiggerAndExpect<AttackEvent>({test.sut_.animationClipsNames_.aim.recoil});
}
}  // namespace

TEST_F(CharacterControllerTests, Recoil_CrouchChangeStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<CrouchChangeStateEvent>(
        {sut_.animationClipsNames_.aim.recoil, sut_.animationClipsNames_.armed.posture.crouched.idle});
}
TEST_F(CharacterControllerTests, Recoil_RotateLeft)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));
    tiggerAndExpect<RotateLeftEvent>(
        {sut_.animationClipsNames_.aim.recoil, sut_.animationClipsNames_.armed.posture.stand.rotate.left});
}
TEST_F(CharacterControllerTests, Recoil_RotateRight)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));
    tiggerAndExpect<RotateRightEvent>(
        {sut_.animationClipsNames_.aim.recoil, sut_.animationClipsNames_.armed.posture.stand.rotate.right});
}
TEST_F(CharacterControllerTests, Recoil_RotateTargetEvent)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent>(
        RotateTargetEvent{targetRotation.value_},
        {sut_.animationClipsNames_.aim.recoil, sut_.animationClipsNames_.armed.posture.stand.rotate.right});
}
TEST_F(CharacterControllerTests, Recoil_WeaponStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<WeaponStateEvent>({sut_.animationClipsNames_.disarm.front()});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
}
TEST_F(CharacterControllerTests, Recoil_AttackEvent)
{
    prepareState(*this);
    tiggerAndExpect<AttackEvent>({sut_.animationClipsNames_.aim.recoil});
}
TEST_F(CharacterControllerTests, Recoil_AimStopEvent)
{
    prepareState(*this);
    tiggerAndExpect<AimStopEvent>({sut_.animationClipsNames_.armed.posture.stand.idle});
}
TEST_F(CharacterControllerTests, Recoil_ReloadArrowEvent)
{
    prepareState(*this);
    tiggerAndExpect<ReloadArrowEvent>({sut_.animationClipsNames_.aim.draw});
}
TEST_F(CharacterControllerTests, Recoil_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent>({sut_.animationClipsNames_.armed.posture.stand.death});
}
TEST_F(CharacterControllerTests, Recoil_MoveForwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<MoveForwardEvent>(
        {sut_.animationClipsNames_.armed.movement.walk.forward, sut_.animationClipsNames_.aim.recoil});
}
TEST_F(CharacterControllerTests, Recoil_RunBackwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(-DEFAULT_BACKWARD_WALK_SPEED);
    tiggerAndExpect<MoveBackwardEvent>(
        {sut_.animationClipsNames_.armed.movement.walk.backward, sut_.animationClipsNames_.aim.recoil});
}
TEST_F(CharacterControllerTests, Recoil_WalkForwardEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_LEFT, vec3(DEFAULT_WALK_LEFT_RIGHT_SPEED, 0.0, 0));
    tiggerAndExpect<MoveLeftEvent>({sut_.animationClipsNames_.armed.movement.walk.left, sut_.animationClipsNames_.aim.recoil});
}
TEST_F(CharacterControllerTests, Recoil_MoveRightEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_RIGHT, vec3(DEFAULT_WALK_LEFT_RIGHT_SPEED, 0.0, 0));
    tiggerAndExpect<MoveRightEvent>({sut_.animationClipsNames_.armed.movement.walk.right, sut_.animationClipsNames_.aim.recoil});
}

TEST_F(CharacterControllerTests, Recoil_MoveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, Recoil_MoveBackwardEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, Recoil_MoveLeftEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, Recoil_WalkChangeStateEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, Recoil_SprintStateChangeEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, Recoil_EndForwardMoveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, Recoil_EndBackwardMoveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, Recoil_EndMoveLeftEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, Recoil_EndMoveRightEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, Recoil_RotateLeftEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, Recoil_RotateRightEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, Recoil_EndRotationEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, Recoil_JumpEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, Recoil_JumpConfirmEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, Recoil_DodgeDiveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, Recoil_DodgeForwardEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, Recoil_DodgeBackwardEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, Recoil_DodgeLeftEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, Recoil_DodgeRightEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, Recoil_DodgeEndEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, Recoil_EndAttackEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, Recoil_EquipEndStateEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, Recoil_DisarmEndStateEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, Recoil_DrawArrowEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, Recoil_AimStartEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, Recoil_StartFallingEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, Recoil_GroundDetectionEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, Recoil_HurtEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, Recoil_AnimationPlayEndEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, Recoil_ChangeAnimEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, Recoil_StartDialogEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, Recoil_EndDialogEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
