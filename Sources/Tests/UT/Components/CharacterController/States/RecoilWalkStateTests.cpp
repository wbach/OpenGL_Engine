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
    test.triggerAndExpect<WeaponStateEvent>(
        {test.sut_.animationClipsNames_.armed.posture.stand.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    test.triggerAndExpect<DrawArrowEvent>({test.sut_.animationClipsNames_.aim.draw});
    test.Update(ADVANCED_TIME_CLIP_TIME);
    test.Update(ADVANCED_TIME_TRANSITION_TIME);

    test.triggerAndExpect<AttackEvent>({test.sut_.animationClipsNames_.aim.recoil});
    test.triggerAndExpect<MoveForwardEvent>(
        {test.sut_.animationClipsNames_.aim.recoil, test.sut_.animationClipsNames_.armed.movement.walk.forward});
}
}  // namespace

TEST_F(CharacterControllerTests, RecoilWalk_CrouchChangeStateEvent)
{
    prepareState(*this);
    triggerAndExpect<CrouchChangeStateEvent>(
        {sut_.animationClipsNames_.aim.recoil, sut_.animationClipsNames_.armed.movement.crouch.forward});
}
TEST_F(CharacterControllerTests, RecoilWalk_RotateLeft)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));
    triggerAndExpect<RotateLeftEvent>(
        {sut_.animationClipsNames_.aim.recoil, sut_.animationClipsNames_.armed.movement.walk.forward});
}
TEST_F(CharacterControllerTests, RecoilWalk_RotateRight)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));
    triggerAndExpect<RotateRightEvent>(
        {sut_.animationClipsNames_.aim.recoil, sut_.animationClipsNames_.armed.movement.walk.forward});
}
TEST_F(CharacterControllerTests, RecoilWalk_RotateTargetEvent)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    triggerAndExpect<RotateTargetEvent>(
        RotateTargetEvent{targetRotation.value_},
        {sut_.animationClipsNames_.aim.recoil, sut_.animationClipsNames_.armed.movement.walk.forward});
}
TEST_F(CharacterControllerTests, RecoilWalk_WeaponStateEvent)
{
    prepareState(*this);
    triggerAndExpect<WeaponStateEvent>(
        {sut_.animationClipsNames_.disarm.front(), sut_.animationClipsNames_.disarmed.movement.walk.forward});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.movement.walk.forward});
}
TEST_F(CharacterControllerTests, RecoilWalk_ReloadArrowEvent)
{
    prepareState(*this);
    triggerAndExpect<ReloadArrowEvent>(
        {sut_.animationClipsNames_.aim.draw, sut_.animationClipsNames_.armed.movement.walk.forward});
}
TEST_F(CharacterControllerTests, RecoilWalk_AimStopEvent)
{
    prepareState(*this);
    triggerAndExpect<AimStopEvent>({sut_.animationClipsNames_.armed.movement.walk.forward});
}
TEST_F(CharacterControllerTests, RecoilWalk_MoveForwardEvent)
{
    prepareState(*this);
    triggerAndExpect<MoveForwardEvent>(
        {sut_.animationClipsNames_.aim.recoil, sut_.animationClipsNames_.armed.movement.walk.forward});
}
TEST_F(CharacterControllerTests, RecoilWalk_MoveBackwardEvent)
{
    prepareState(*this);
    expectNoMove();
    triggerAndExpect<MoveBackwardEvent>({sut_.animationClipsNames_.aim.recoil});
}
TEST_F(CharacterControllerTests, RecoilWalk_DeathEvent)
{
    prepareState(*this);
    triggerAndExpect<DeathEvent>({sut_.animationClipsNames_.armed.posture.stand.death});
}
TEST_F(CharacterControllerTests, RecoilWalk_SprintStateChangeEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    triggerAndExpect<SprintStateChangeEvent>({sut_.animationClipsNames_.armed.sprint});
}
TEST_F(CharacterControllerTests, RecoilWalk_WalkChangeStateEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    triggerAndExpect<WalkChangeStateEvent>(
        {sut_.animationClipsNames_.aim.recoil, sut_.animationClipsNames_.armed.movement.walk.forward});
}
TEST_F(CharacterControllerTests, RecoilWalk_MoveLeftEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_FORWARD + VECTOR_LEFT, vec3(DEFAULT_WALK_LEFT_RIGHT_SPEED, 0.0, DEFAULT_WALK_SPEED));
    triggerAndExpect<MoveLeftEvent>(
        {sut_.animationClipsNames_.aim.recoil, sut_.animationClipsNames_.armed.movement.walk.forward});
}
TEST_F(CharacterControllerTests, RecoilWalk_RunBackwardEvent)
{
    prepareState(*this);
    expectNoMove();
    triggerAndExpect<MoveBackwardEvent>({sut_.animationClipsNames_.aim.recoil});
    expectVelocity(VECTOR_BACKWARD, vec3(0.0, 0.0, DEFAULT_BACKWARD_WALK_SPEED));
    triggerAndExpect<EndForwardMoveEvent>(
        {sut_.animationClipsNames_.aim.recoil, sut_.animationClipsNames_.armed.movement.walk.backward});
}
TEST_F(CharacterControllerTests, RecoilWalk_EndForwardMoveEvent)
{
    prepareState(*this);
    triggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.aim.recoil});
}
TEST_F(CharacterControllerTests, RecoilWalk_EndBackwardMoveEvent)
{
    prepareState(*this);
    triggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.aim.recoil});
    expectForwardVelocity(-DEFAULT_BACKWARD_WALK_SPEED);
    triggerAndExpect<MoveBackwardEvent>(
        {sut_.animationClipsNames_.aim.recoil, sut_.animationClipsNames_.armed.movement.walk.backward});
    triggerAndExpect<EndBackwardMoveEvent>({sut_.animationClipsNames_.aim.recoil});
}
TEST_F(CharacterControllerTests, RecoilWalk_ReloadArrowEventWhenRunLeftAndRight)
{
    prepareState(*this);
    triggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.aim.recoil});
    expectVelocity(VECTOR_LEFT, vec3(DEFAULT_WALK_LEFT_RIGHT_SPEED, 0.f, 0.f));
    triggerAndExpect<MoveLeftEvent>({sut_.animationClipsNames_.aim.recoil, sut_.animationClipsNames_.armed.movement.walk.left});
    expectNoMove();
    triggerAndExpect<MoveRightEvent>({sut_.animationClipsNames_.aim.recoil});
    expectNoMove();
    triggerAndExpect<ReloadArrowEvent>({sut_.animationClipsNames_.aim.draw});
}

TEST_F(CharacterControllerTests, RecoilWalk_MoveEvent)
{
    prepareState(*this);
    // TODO: Implement test for MoveEvent
}
TEST_F(CharacterControllerTests, RecoilWalk_MoveRightEvent)
{
    prepareState(*this);
    // TODO: Implement test for MoveRightEvent
}
TEST_F(CharacterControllerTests, RecoilWalk_EndMoveLeftEvent)
{
    prepareState(*this);
    // TODO: Implement test for EndMoveLeftEvent
}
TEST_F(CharacterControllerTests, RecoilWalk_EndMoveRightEvent)
{
    prepareState(*this);
    // TODO: Implement test for EndMoveRightEvent
}
TEST_F(CharacterControllerTests, RecoilWalk_RotateLeftEvent)
{
    prepareState(*this);
    // TODO: Implement test for RotateLeftEvent
}
TEST_F(CharacterControllerTests, RecoilWalk_RotateRightEvent)
{
    prepareState(*this);
    // TODO: Implement test for RotateRightEvent
}
TEST_F(CharacterControllerTests, RecoilWalk_EndRotationEvent)
{
    prepareState(*this);
    // TODO: Implement test for EndRotationEvent
}
TEST_F(CharacterControllerTests, RecoilWalk_JumpEvent)
{
    prepareState(*this);
    // TODO: Implement test for JumpEvent
}
TEST_F(CharacterControllerTests, RecoilWalk_JumpConfirmEvent)
{
    prepareState(*this);
    // TODO: Implement test for JumpConfirmEvent
}
TEST_F(CharacterControllerTests, RecoilWalk_DodgeDiveEvent)
{
    prepareState(*this);
    // TODO: Implement test for DodgeDiveEvent
}
TEST_F(CharacterControllerTests, RecoilWalk_DodgeForwardEvent)
{
    prepareState(*this);
    // TODO: Implement test for DodgeForwardEvent
}
TEST_F(CharacterControllerTests, RecoilWalk_DodgeBackwardEvent)
{
    prepareState(*this);
    // TODO: Implement test for DodgeBackwardEvent
}
TEST_F(CharacterControllerTests, RecoilWalk_DodgeLeftEvent)
{
    prepareState(*this);
    // TODO: Implement test for DodgeLeftEvent
}
TEST_F(CharacterControllerTests, RecoilWalk_DodgeRightEvent)
{
    prepareState(*this);
    // TODO: Implement test for DodgeRightEvent
}
TEST_F(CharacterControllerTests, RecoilWalk_DodgeEndEvent)
{
    prepareState(*this);
    // TODO: Implement test for DodgeEndEvent
}
TEST_F(CharacterControllerTests, RecoilWalk_AttackEvent)
{
    prepareState(*this);
    // TODO: Implement test for AttackEvent
}
TEST_F(CharacterControllerTests, RecoilWalk_EndAttackEvent)
{
    prepareState(*this);
    // TODO: Implement test for EndAttackEvent
}
TEST_F(CharacterControllerTests, RecoilWalk_EquipEndStateEvent)
{
    prepareState(*this);
    // TODO: Implement test for EquipEndStateEvent
}
TEST_F(CharacterControllerTests, RecoilWalk_DisarmEndStateEvent)
{
    prepareState(*this);
    // TODO: Implement test for DisarmEndStateEvent
}
TEST_F(CharacterControllerTests, RecoilWalk_DrawArrowEvent)
{
    prepareState(*this);
    // TODO: Implement test for DrawArrowEvent
}
TEST_F(CharacterControllerTests, RecoilWalk_AimStartEvent)
{
    prepareState(*this);
    // TODO: Implement test for AimStartEvent
}
TEST_F(CharacterControllerTests, RecoilWalk_StartFallingEvent)
{
    prepareState(*this);
    triggerAndExpect<StartFallingEvent>({sut_.animationClipsNames_.armed.falling});
}
TEST_F(CharacterControllerTests, RecoilWalk_GroundDetectionEvent)
{
    prepareState(*this);
    // TODO: Implement test for GroundDetectionEvent
}
TEST_F(CharacterControllerTests, RecoilWalk_HurtEvent)
{
    prepareState(*this);
    triggerAndExpect<HurtEvent>({sut_.animationClipsNames_.armed.posture.stand.hurt});
}
TEST_F(CharacterControllerTests, RecoilWalk_AnimationPlayEndEvent)
{
    prepareState(*this);
    // TODO: Implement test for AnimationPlayEndEvent
}
TEST_F(CharacterControllerTests, RecoilWalk_ChangeAnimEvent)
{
    prepareState(*this);
    // TODO: Implement test for ChangeAnimEvent
}
TEST_F(CharacterControllerTests, RecoilWalk_StartDialogEvent)
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
TEST_F(CharacterControllerTests, RecoilWalk_EndDialogEvent)
{
    prepareState(*this);
    // TODO: Implement test for EndDialogEvent
}
