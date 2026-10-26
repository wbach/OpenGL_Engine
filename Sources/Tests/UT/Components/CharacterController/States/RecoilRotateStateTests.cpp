#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    EXPECT_CALL(test.inputManagerMock_, CalcualteMouseMove()).WillRepeatedly(Return(vec2i{0, 0}));
    EXPECT_CALL(test.physicsApiMock_, SetRotation(test.rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));
    EXPECT_CALL(test.physicsApiMock_, GetRotation(test.rigidbodyid)).WillRepeatedly(Return(Rotation().value_));

    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.posture.stand.idle});
    test.tiggerAndExpect<WeaponStateEvent>(
        {test.sut_.animationClipsNames_.armed.posture.stand.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    test.tiggerAndExpect<DrawArrowEvent>({test.sut_.animationClipsNames_.aim.draw});
    test.Update(ADVANCED_TIME_CLIP_TIME);
    test.Update(ADVANCED_TIME_TRANSITION_TIME);
    test.tiggerAndExpect<AttackEvent>({test.sut_.animationClipsNames_.aim.recoil});
    test.tiggerAndExpect<RotateLeftEvent>(
        {test.sut_.animationClipsNames_.aim.recoil, test.sut_.animationClipsNames_.armed.posture.stand.rotate.left});
}
}  // namespace

TEST_F(CharacterControllerTests, RecoilRotate_CrouchChangeStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<CrouchChangeStateEvent>(
        {sut_.animationClipsNames_.aim.recoil, sut_.animationClipsNames_.armed.posture.crouched.rotate.left});
}
TEST_F(CharacterControllerTests, RecoilRotate_RotateLeft)
{
    prepareState(*this);
    tiggerAndExpect<RotateLeftEvent>(
        {sut_.animationClipsNames_.aim.recoil, sut_.animationClipsNames_.armed.posture.stand.rotate.left});
}
TEST_F(CharacterControllerTests, RecoilRotate_RotateRight)
{
    prepareState(*this);
    tiggerAndExpect<RotateRightEvent>(
        {sut_.animationClipsNames_.aim.recoil, sut_.animationClipsNames_.armed.posture.stand.rotate.right});
}
TEST_F(CharacterControllerTests, RecoilRotate_RotateTargetEvent)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent>(
        RotateTargetEvent{targetRotation.value_},
        {sut_.animationClipsNames_.aim.recoil, sut_.animationClipsNames_.armed.posture.stand.rotate.right});
}
TEST_F(CharacterControllerTests, RecoilRotate_WeaponStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<WeaponStateEvent>(
        {sut_.animationClipsNames_.disarm.front(), sut_.animationClipsNames_.disarmed.posture.stand.rotate.left});

    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectRotationLeft(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.rotate.left});
}
TEST_F(CharacterControllerTests, RecoilRotate_AttackEvent)
{
    prepareState(*this);
    tiggerAndExpect<AttackEvent>(
        {sut_.animationClipsNames_.armed.posture.stand.rotate.left, sut_.animationClipsNames_.aim.recoil});
}
TEST_F(CharacterControllerTests, RecoilRotate_EndRotationEvent)
{
    prepareState(*this);
    tiggerAndExpect<EndRotationEvent>({sut_.animationClipsNames_.aim.recoil});
}
TEST_F(CharacterControllerTests, RecoilRotate_AimStopEvent)
{
    prepareState(*this);
    tiggerAndExpect<AimStopEvent>({sut_.animationClipsNames_.armed.posture.stand.rotate.left});
}
TEST_F(CharacterControllerTests, RecoilRotate_ReloadArrowEvent)
{
    prepareState(*this);
    tiggerAndExpect<ReloadArrowEvent>(
        {sut_.animationClipsNames_.armed.posture.stand.rotate.left, sut_.animationClipsNames_.aim.draw});
}
TEST_F(CharacterControllerTests, RecoilRotate_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent>({sut_.animationClipsNames_.armed.posture.stand.death});
}
TEST_F(CharacterControllerTests, RecoilRotate_MoveForwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<MoveForwardEvent>(
        {sut_.animationClipsNames_.aim.recoil, sut_.animationClipsNames_.armed.movement.walk.forward});
}
TEST_F(CharacterControllerTests, RecoilRotate_MoveBackwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(-DEFAULT_BACKWARD_WALK_SPEED);
    tiggerAndExpect<MoveBackwardEvent>(
        {sut_.animationClipsNames_.aim.recoil, sut_.animationClipsNames_.armed.movement.walk.backward});
}
TEST_F(CharacterControllerTests, RecoilRotate_MoveLeftEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_LEFT, vec3(DEFAULT_WALK_LEFT_RIGHT_SPEED, 0.0, 0));
    tiggerAndExpect<MoveLeftEvent>({sut_.animationClipsNames_.aim.recoil, sut_.animationClipsNames_.armed.movement.walk.left});
}
TEST_F(CharacterControllerTests, RecoilRotate_MoveRightEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_RIGHT, vec3(DEFAULT_WALK_LEFT_RIGHT_SPEED, 0.0, 0));
    tiggerAndExpect<MoveRightEvent>({sut_.animationClipsNames_.aim.recoil, sut_.animationClipsNames_.armed.movement.walk.right});
}
TEST_F(CharacterControllerTests, RecoilRotate_RotateLeftEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilRotate_RotateRightEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilRotate_MoveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilRotate_WalkChangeStateEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilRotate_SprintStateChangeEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilRotate_EndForwardMoveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilRotate_EndBackwardMoveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilRotate_EndMoveLeftEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilRotate_EndMoveRightEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilRotate_JumpEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilRotate_JumpConfirmEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilRotate_DodgeDiveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilRotate_DodgeForwardEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilRotate_DodgeBackwardEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilRotate_DodgeLeftEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilRotate_DodgeRightEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilRotate_DodgeEndEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilRotate_EndAttackEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilRotate_EquipEndStateEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilRotate_DisarmEndStateEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilRotate_DrawArrowEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilRotate_AimStartEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilRotate_StartFallingEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilRotate_GroundDetectionEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilRotate_HurtEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilRotate_AnimationPlayEndEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilRotate_ChangeAnimEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilRotate_StartDialogEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, RecoilRotate_EndDialogEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
