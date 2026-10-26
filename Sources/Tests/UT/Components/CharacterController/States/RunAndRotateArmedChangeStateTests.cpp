#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.posture.stand.idle});
    test.expectForwardVelocity(DEFAULT_RUN_SPEED);
    test.expectRotationRight();
    test.tiggerAndExpect<MoveForwardEvent>({test.sut_.animationClipsNames_.disarmed.movement.run.forward});
    test.tiggerAndExpect<RotateRightEvent>({test.sut_.animationClipsNames_.disarmed.movement.run.forward});
    test.tiggerAndExpect<WeaponStateEvent>(
        {test.sut_.animationClipsNames_.armed.movement.run.forward, test.sut_.animationClipsNames_.equip.front()});
}
}  // namespace

// TEST_F(CharacterControllerTests, RunAndRotateArmedChangeState_WeaponStateEvent)
//{
//     prepareState(*this);
//     tiggerAndExpect<WeaponStateEvent, IdleArmedChangeState>({sut_.animationClipsNames_.disarm.front()},
//                                                             {ADVANCED_TIME_TRANSITION_TIME});
//     Update(ADVANCED_TIME_CLIP_TIME);
//     Update(ADVANCED_TIME_TRANSITION_TIME);
//     expectState<DisarmedIdleState>();
// }
TEST_F(CharacterControllerTests, RunAndRotateArmedChangeState_CrouchChangeStateEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<CrouchChangeStateEvent>(
        {sut_.animationClipsNames_.armed.movement.crouch.forward, sut_.animationClipsNames_.equip.front()});
}
TEST_F(CharacterControllerTests, RunAndRotateArmedChangeState_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent>({sut_.animationClipsNames_.disarmed.posture.stand.death});
}
TEST_F(CharacterControllerTests, RunAndRotateArmedChangeState_EquipEndStateEvent)
{
    prepareState(*this);
    expectRotationRight(ADVANCED_TIME_CLIP_TIME);
    tiggerAndExpect<EquipEndStateEvent>({sut_.animationClipsNames_.armed.movement.run.forward},
                                        {ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_CLIP_TIME});
}
TEST_F(CharacterControllerTests, RunAndRotateArmedChangeState_DisarmEndStateEvent)
{
    prepareState(*this);
    expectRotationRight(ADVANCED_TIME_CLIP_TIME);
    tiggerAndExpect<DisarmEndStateEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward},
                                         {ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_CLIP_TIME});
}
TEST_F(CharacterControllerTests, RunAndRotateArmedChangeState_MoveForwardEvent)
{
    prepareState(*this);
    tiggerAndExpect<MoveForwardEvent>(
        {sut_.animationClipsNames_.equip.front(), sut_.animationClipsNames_.armed.movement.run.forward});
}
TEST_F(CharacterControllerTests, RunAndRotateArmedChangeState_MoveBackwardEvent)
{
    prepareState(*this);
    expectNoMove();
    tiggerAndExpect<MoveBackwardEvent>(
        {sut_.animationClipsNames_.equip.front(), sut_.animationClipsNames_.disarmed.posture.stand.rotate.right});
}
TEST_F(CharacterControllerTests, RunAndRotateArmedChangeState_MoveLeftEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_FORWARD + VECTOR_LEFT, vec3(DEFAULT_RUN_SPEED, 0.0, DEFAULT_RUN_SPEED));
    tiggerAndExpect<MoveLeftEvent>(
        {sut_.animationClipsNames_.equip.front(), sut_.animationClipsNames_.armed.movement.run.forward});
    expectRotationRight(DEFAULT_MOVING_CHANGE_DIR_SPEED);
    expectRootboneRotation(VECTOR_FORWARD + VECTOR_LEFT);
}
TEST_F(CharacterControllerTests, RunAndRotateArmedChangeState_MoveRightEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_FORWARD + VECTOR_RIGHT, vec3(DEFAULT_RUN_SPEED, 0.0, DEFAULT_RUN_SPEED));
    tiggerAndExpect<MoveRightEvent>(
        {sut_.animationClipsNames_.equip.front(), sut_.animationClipsNames_.armed.movement.run.forward});
    expectRotationRight(DEFAULT_MOVING_CHANGE_DIR_SPEED);
    expectRootboneRotation(VECTOR_FORWARD + VECTOR_RIGHT);
}
TEST_F(CharacterControllerTests, RunAndRotateArmedChangeState_WalkChangeStateEventt)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent>(
        {sut_.animationClipsNames_.equip.front(), sut_.animationClipsNames_.armed.movement.walk.forward});
}
TEST_F(CharacterControllerTests, RunAndRotateArmedChangeState_SprintStateChangeEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<SprintStateChangeEvent>(
        {sut_.animationClipsNames_.equip.front(), sut_.animationClipsNames_.armed.movement.run.forward},
        {ADVANCED_TIME_TRANSITION_TIME});

    expectRotationRight(ADVANCED_TIME_CLIP_TIME);
    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectRotationRight();
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.sprint});
}
TEST_F(CharacterControllerTests, RunAndRotateArmedChangeState_RotateLeftEvent)
{
    prepareState(*this);
    expectRotationLeft();
    tiggerAndExpect<RotateLeftEvent>(
        {sut_.animationClipsNames_.equip.front(), sut_.animationClipsNames_.armed.movement.run.forward});
}
TEST_F(CharacterControllerTests, RunAndRotateArmedChangeState_RotateRightEvent)
{
    prepareState(*this);
    expectRotationRight();
    tiggerAndExpect<RotateRightEvent>(
        {sut_.animationClipsNames_.equip.front(), sut_.animationClipsNames_.armed.movement.run.forward});
}
TEST_F(CharacterControllerTests, RunAndRotateArmedChangeState_RotateTargetEvent)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent>(
        RotateTargetEvent{targetRotation.value_},
        {sut_.animationClipsNames_.equip.front(), sut_.animationClipsNames_.armed.movement.run.forward});
}
TEST_F(CharacterControllerTests, RunAndRotateArmedChangeState_DrawArrowEvent)
{
    prepareState(*this);
    tiggerAndExpect<DrawArrowEvent>(
        {sut_.animationClipsNames_.armed.movement.run.forward, sut_.animationClipsNames_.equip.front()});
    expectRotationRight(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectRotationRight();
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
}
TEST_F(CharacterControllerTests, RunAndRotateArmedChangeState_AimStopEvent)
{
    prepareState(*this);
    tiggerAndExpect<DrawArrowEvent>(
        {sut_.animationClipsNames_.armed.movement.run.forward, sut_.animationClipsNames_.equip.front()});
    tiggerAndExpect<AimStopEvent>(
        {sut_.animationClipsNames_.armed.movement.run.forward, sut_.animationClipsNames_.equip.front()});
    expectRotationRight(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectRotationRight();
    Update(ADVANCED_TIME_TRANSITION_TIME);
}

TEST_F(CharacterControllerTests, RunAndRotateArmedChangeState_MoveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, RunAndRotateArmedChangeState_EndForwardMoveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, RunAndRotateArmedChangeState_EndBackwardMoveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, RunAndRotateArmedChangeState_EndMoveLeftEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, RunAndRotateArmedChangeState_EndMoveRightEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, RunAndRotateArmedChangeState_EndRotationEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, RunAndRotateArmedChangeState_JumpEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, RunAndRotateArmedChangeState_JumpConfirmEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, RunAndRotateArmedChangeState_DodgeDiveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, RunAndRotateArmedChangeState_DodgeForwardEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, RunAndRotateArmedChangeState_DodgeBackwardEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, RunAndRotateArmedChangeState_DodgeLeftEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, RunAndRotateArmedChangeState_DodgeRightEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, RunAndRotateArmedChangeState_DodgeEndEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, RunAndRotateArmedChangeState_AttackEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, RunAndRotateArmedChangeState_EndAttackEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, RunAndRotateArmedChangeState_ReloadArrowEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, RunAndRotateArmedChangeState_AimStartEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, RunAndRotateArmedChangeState_StartFallingEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, RunAndRotateArmedChangeState_GroundDetectionEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, RunAndRotateArmedChangeState_HurtEvent)
{
    prepareState(*this);
    tiggerAndExpect<HurtEvent>({sut_.animationClipsNames_.disarmed.posture.stand.hurt});
}

TEST_F(CharacterControllerTests, RunAndRotateArmedChangeState_AnimationPlayEndEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, RunAndRotateArmedChangeState_ChangeAnimEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, RunAndRotateArmedChangeState_StartDialogEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, RunAndRotateArmedChangeState_EndDialogEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
