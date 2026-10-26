#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.posture.stand.idle});
    test.expectForwardVelocity(DEFAULT_RUN_SPEED);
    test.tiggerAndExpect<MoveForwardEvent>({test.sut_.animationClipsNames_.disarmed.movement.run.forward});
    test.expectForwardVelocity(DEFAULT_WALK_SPEED);
    test.tiggerAndExpect<CrouchChangeStateEvent>({test.sut_.animationClipsNames_.disarmed.movement.crouch.forward});
    test.tiggerAndExpect<WeaponStateEvent>(
        {test.sut_.animationClipsNames_.armed.movement.crouch.forward, test.sut_.animationClipsNames_.equip.front()});
}
}  // namespace

// TEST_F(CharacterControllerTests, CrouchWalkArmedChangeState_WeaponStateEvent)
//{
//     prepareState(*this);
//     tiggerAndExpect<WeaponStateEvent, IdleArmedChangeState>({sut_.animationClipsNames_.disarm.front()},
//                                                             {ADVANCED_TIME_TRANSITION_TIME});
//     Update(ADVANCED_TIME_CLIP_TIME);
//     Update(ADVANCED_TIME_TRANSITION_TIME);
//     expectState<DisarmedIdleState>();
// }
TEST_F(CharacterControllerTests, CrouchWalkArmedChangeState_CrouchChangeStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<CrouchChangeStateEvent>(
        {sut_.animationClipsNames_.armed.movement.walk.forward, sut_.animationClipsNames_.equip.front()});
}
TEST_F(CharacterControllerTests, CrouchWalkArmedChangeState_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent>({sut_.animationClipsNames_.disarmed.posture.crouched.death});
}
TEST_F(CharacterControllerTests, CrouchWalkArmedChangeState_EquipEndStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<EquipEndStateEvent>({sut_.animationClipsNames_.armed.movement.crouch.forward},
                                        {ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
}
TEST_F(CharacterControllerTests, CrouchWalkArmedChangeState_DisarmEndStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<DisarmEndStateEvent>({sut_.animationClipsNames_.disarmed.movement.crouch.forward},
                                         {ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
}
TEST_F(CharacterControllerTests, CrouchWalkArmedChangeState_MoveForwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<MoveForwardEvent>(
        {sut_.animationClipsNames_.equip.front(), sut_.animationClipsNames_.armed.movement.crouch.forward});
}
TEST_F(CharacterControllerTests, CrouchWalkArmedChangeState_MoveBackwardEvent)
{
    prepareState(*this);
    expectNoMove();
    tiggerAndExpect<MoveBackwardEvent>({sut_.animationClipsNames_.equip.front()});
}
TEST_F(CharacterControllerTests, CrouchWalkArmedChangeState_MoveLeftEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_FORWARD + VECTOR_LEFT, vec3(DEFAULT_WALK_SPEED, 0.0, DEFAULT_WALK_SPEED));
    tiggerAndExpect<MoveLeftEvent>(
        {sut_.animationClipsNames_.equip.front(), sut_.animationClipsNames_.armed.movement.crouch.forward});
    expectRootboneRotation(VECTOR_FORWARD + VECTOR_LEFT);
}
TEST_F(CharacterControllerTests, CrouchWalkArmedChangeState_MoveRightEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_FORWARD + VECTOR_RIGHT, vec3(DEFAULT_WALK_SPEED, 0.0, DEFAULT_WALK_SPEED));
    tiggerAndExpect<MoveRightEvent>(
        {sut_.animationClipsNames_.equip.front(), sut_.animationClipsNames_.armed.movement.crouch.forward});
    expectRootboneRotation(VECTOR_FORWARD + VECTOR_RIGHT);
}
TEST_F(CharacterControllerTests, CrouchWalkArmedChangeState_WalkChangeStateEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<WalkChangeStateEvent>(
        {sut_.animationClipsNames_.equip.front(), sut_.animationClipsNames_.armed.movement.run.forward});
}
TEST_F(CharacterControllerTests, CrouchWalkArmedChangeState_SprintStateChangeEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<SprintStateChangeEvent>(
        {sut_.animationClipsNames_.equip.front(), sut_.animationClipsNames_.armed.movement.run.forward},
        {ADVANCED_TIME_TRANSITION_TIME});

    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.sprint});
}
TEST_F(CharacterControllerTests, CrouchWalkArmedChangeState_RotateLeftEvent)
{
    prepareState(*this);
    expectRotationLeft();
    tiggerAndExpect<RotateLeftEvent>(
        {sut_.animationClipsNames_.equip.front(), sut_.animationClipsNames_.armed.movement.crouch.forward});
}
TEST_F(CharacterControllerTests, CrouchWalkArmedChangeState_RotateRightEvent)
{
    prepareState(*this);
    expectRotationRight();
    tiggerAndExpect<RotateRightEvent>(
        {sut_.animationClipsNames_.equip.front(), sut_.animationClipsNames_.armed.movement.crouch.forward});
}
TEST_F(CharacterControllerTests, CrouchWalkArmedChangeState_RotateTargetEvent)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent>(
        RotateTargetEvent{targetRotation.value_},
        {sut_.animationClipsNames_.equip.front(), sut_.animationClipsNames_.armed.movement.crouch.forward});
}
TEST_F(CharacterControllerTests, CrouchWalkArmedChangeState_DrawArrowEvent)
{
    prepareState(*this);
    tiggerAndExpect<DrawArrowEvent>(
        {sut_.animationClipsNames_.armed.movement.crouch.forward, sut_.animationClipsNames_.equip.front()});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.movement.crouch.forward, sut_.animationClipsNames_.aim.draw});
}
TEST_F(CharacterControllerTests, CrouchWalkArmedChangeState_AimStopEvent)
{
    prepareState(*this);
    tiggerAndExpect<DrawArrowEvent>(
        {sut_.animationClipsNames_.armed.movement.crouch.forward, sut_.animationClipsNames_.equip.front()});
    tiggerAndExpect<AimStopEvent>(
        {sut_.animationClipsNames_.armed.movement.crouch.forward, sut_.animationClipsNames_.equip.front()});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.movement.crouch.forward});
}

TEST_F(CharacterControllerTests, CrouchWalkArmedChangeState_MoveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, CrouchWalkArmedChangeState_EndForwardMoveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, CrouchWalkArmedChangeState_EndBackwardMoveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, CrouchWalkArmedChangeState_EndMoveLeftEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, CrouchWalkArmedChangeState_EndMoveRightEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, CrouchWalkArmedChangeState_EndRotationEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, CrouchWalkArmedChangeState_JumpEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, CrouchWalkArmedChangeState_JumpConfirmEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, CrouchWalkArmedChangeState_DodgeDiveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, CrouchWalkArmedChangeState_DodgeForwardEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, CrouchWalkArmedChangeState_DodgeBackwardEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, CrouchWalkArmedChangeState_DodgeLeftEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, CrouchWalkArmedChangeState_DodgeRightEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, CrouchWalkArmedChangeState_DodgeEndEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, CrouchWalkArmedChangeState_AttackEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, CrouchWalkArmedChangeState_EndAttackEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, CrouchWalkArmedChangeState_ReloadArrowEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, CrouchWalkArmedChangeState_AimStartEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, CrouchWalkArmedChangeState_StartFallingEvent)
{
    prepareState(*this);
    tiggerAndExpect<StartFallingEvent>({sut_.animationClipsNames_.disarmed.falling});
}

TEST_F(CharacterControllerTests, CrouchWalkArmedChangeState_GroundDetectionEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, CrouchWalkArmedChangeState_HurtEvent)
{
    prepareState(*this);
    tiggerAndExpect<HurtEvent>({sut_.animationClipsNames_.disarmed.posture.crouched.hurt});
}

TEST_F(CharacterControllerTests, CrouchWalkArmedChangeState_AnimationPlayEndEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, CrouchWalkArmedChangeState_ChangeAnimEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, CrouchWalkArmedChangeState_StartDialogEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, CrouchWalkArmedChangeState_EndDialogEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
