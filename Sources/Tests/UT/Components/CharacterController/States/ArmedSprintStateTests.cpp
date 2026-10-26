#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    EXPECT_CALL(test.physicsApiMock_, GetVelocity(test.rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(test.physicsApiMock_, GetRotation(test.rigidbodyid)).WillRepeatedly(Return(Rotation().value_));

    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.posture.stand.idle});
    test.tiggerAndExpect<WeaponStateEvent>(
        {test.sut_.animationClipsNames_.armed.posture.stand.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
    test.expectForwardVelocity(DEFAULT_RUN_SPEED);
    test.tiggerAndExpect<MoveForwardEvent>({test.sut_.animationClipsNames_.armed.movement.run.forward});
    test.expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    test.tiggerAndExpect<SprintStateChangeEvent>({test.sut_.animationClipsNames_.armed.sprint});
}
}  // namespace

TEST_F(CharacterControllerTests, ArmedSprintState_CrouchChangeStateEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<CrouchChangeStateEvent>({sut_.animationClipsNames_.armed.movement.crouch.forward});
}
TEST_F(CharacterControllerTests, ArmedSprintState_DrawArrowEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<DrawArrowEvent>({sut_.animationClipsNames_.armed.movement.walk.forward, sut_.animationClipsNames_.aim.draw});
}
TEST_F(CharacterControllerTests, ArmedSprintState_MoveForwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<MoveForwardEvent>({sut_.animationClipsNames_.armed.sprint});
}
TEST_F(CharacterControllerTests, ArmedSprintState_MoveBackwardEvent)
{
    prepareState(*this);
    expectNoMove();
    tiggerAndExpect<MoveBackwardEvent>({sut_.animationClipsNames_.armed.posture.stand.idle});
}
TEST_F(CharacterControllerTests, ArmedSprintState_EndBackwardMoveEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    tiggerAndExpect<WeaponStateEvent>({sut_.animationClipsNames_.armed.posture.stand.idle},
                                      {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
    expectForwardVelocity(-DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveBackwardEvent>({sut_.animationClipsNames_.armed.movement.run.forward});
    expectRootboneRotation(VECTOR_BACKWARD);
    expectForwardVelocity(-DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent>({sut_.animationClipsNames_.armed.sprint});
    expectRootboneRotation(VECTOR_BACKWARD);
    tiggerAndExpect<EndBackwardMoveEvent>({sut_.animationClipsNames_.armed.posture.stand.idle});
}
TEST_F(CharacterControllerTests, ArmedSprintState_MoveLeftEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_FORWARD + VECTOR_LEFT, vec3(DEFAULT_SPRINT_SPEED, 0.0, DEFAULT_SPRINT_SPEED));
    tiggerAndExpect<MoveLeftEvent>({sut_.animationClipsNames_.armed.sprint});
    expectRootboneRotation(VECTOR_FORWARD + VECTOR_LEFT);
}

TEST_F(CharacterControllerTests, ArmedSprintState_MoveRightEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_FORWARD + VECTOR_RIGHT, vec3(DEFAULT_SPRINT_SPEED, 0.0, DEFAULT_SPRINT_SPEED));
    tiggerAndExpect<MoveRightEvent>({sut_.animationClipsNames_.armed.sprint});
    expectRootboneRotation(VECTOR_FORWARD + VECTOR_RIGHT);
}
TEST_F(CharacterControllerTests, ArmedSprintState_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent>({sut_.animationClipsNames_.armed.posture.stand.death});
}
TEST_F(CharacterControllerTests, ArmedSprintState_WalkChangeStateEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent>({sut_.animationClipsNames_.armed.movement.walk.forward});
}
TEST_F(CharacterControllerTests, ArmedSprintState_SprintStateChangeEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<SprintStateChangeEvent>({sut_.animationClipsNames_.armed.movement.run.forward});
}
TEST_F(CharacterControllerTests, ArmedSprintState_WeaponStateEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<WeaponStateEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward},
                                      {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
}
TEST_F(CharacterControllerTests, ArmedSprintState_EndForwardMoveEvent)
{
    prepareState(*this);
    tiggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.armed.posture.stand.idle});
}
TEST_F(CharacterControllerTests, ArmedSprintState_RotateLeftEvent)
{
    prepareState(*this);
    expectRotationLeft();
    tiggerAndExpect<RotateLeftEvent>({sut_.animationClipsNames_.armed.sprint});
}
TEST_F(CharacterControllerTests, ArmedSprintState_RotateRightEvent)
{
    prepareState(*this);
    expectRotationRight();
    tiggerAndExpect<RotateRightEvent>({sut_.animationClipsNames_.armed.sprint});
}
TEST_F(CharacterControllerTests, ArmedSprintState_RotateTargetEvent)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent>(RotateTargetEvent{targetRotation.value_}, {sut_.animationClipsNames_.armed.sprint});
}
TEST_F(CharacterControllerTests, ArmedSprintState_MoveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, ArmedSprintState_EndMoveLeftEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, ArmedSprintState_EndMoveRightEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, ArmedSprintState_EndRotationEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, ArmedSprintState_JumpEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, ArmedSprintState_JumpConfirmEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, ArmedSprintState_DodgeDiveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, ArmedSprintState_DodgeForwardEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, ArmedSprintState_DodgeBackwardEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, ArmedSprintState_DodgeLeftEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, ArmedSprintState_DodgeRightEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, ArmedSprintState_DodgeEndEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, ArmedSprintState_AttackEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, ArmedSprintState_EndAttackEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, ArmedSprintState_EquipEndStateEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, ArmedSprintState_DisarmEndStateEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, ArmedSprintState_ReloadArrowEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, ArmedSprintState_AimStartEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, ArmedSprintState_AimStopEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, ArmedSprintState_StartFallingEvent)
{
    prepareState(*this);
    tiggerAndExpect<StartFallingEvent>({sut_.animationClipsNames_.armed.falling});
}
TEST_F(CharacterControllerTests, ArmedSprintState_GroundDetectionEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, ArmedSprintState_HurtEvent)
{
    prepareState(*this);
    tiggerAndExpect<HurtEvent>({sut_.animationClipsNames_.armed.posture.stand.hurt});
}
TEST_F(CharacterControllerTests, ArmedSprintState_AnimationPlayEndEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, ArmedSprintState_ChangeAnimEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, ArmedSprintState_StartDialogEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, ArmedSprintState_EndDialogEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
