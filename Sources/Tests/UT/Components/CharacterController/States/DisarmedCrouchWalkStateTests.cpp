#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    EXPECT_CALL(test.physicsApiMock_, GetVelocity(test.rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(test.physicsApiMock_, GetRotation(test.rigidbodyid)).WillRepeatedly(Return(Rotation().value_));

    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.posture.stand.idle});

    test.expectForwardVelocity(DEFAULT_RUN_SPEED);
    test.triggerAndExpect<MoveForwardEvent>({test.sut_.animationClipsNames_.disarmed.movement.run.forward});
    test.expectForwardVelocity(DEFAULT_WALK_SPEED);
    test.triggerAndExpect<CrouchChangeStateEvent>({test.sut_.animationClipsNames_.disarmed.movement.crouch.forward});
}
}  // namespace

TEST_F(CharacterControllerTests, DisarmedCruchWalkState_DrawArrowEvent)
{
    prepareState(*this);
    triggerAndExpect<DrawArrowEvent>(
        {sut_.animationClipsNames_.armed.movement.crouch.forward, sut_.animationClipsNames_.equip.front()},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME / 2.f});
    Update(ADVANCED_TIME_CLIP_TIME / 2.f);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.movement.crouch.forward, sut_.animationClipsNames_.aim.draw});
}

TEST_F(CharacterControllerTests, DisarmedCruchWalkState_AimStopDuringDrawArrowEvent)
{
    prepareState(*this);
    triggerAndExpect<DrawArrowEvent>(
        {sut_.animationClipsNames_.armed.movement.crouch.forward, sut_.animationClipsNames_.equip.front()},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME / 2.f});
    triggerAndExpect<AimStopEvent>({sut_.animationClipsNames_.armed.movement.crouch.forward},
                                  {ADVANCED_TIME_CLIP_TIME / 2.f, ADVANCED_TIME_TRANSITION_TIME});

    // Extra check
    Update(ADVANCED_TIME_CLIP_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.movement.crouch.forward});
}

TEST_F(CharacterControllerTests, DisarmedCruchWalkState_CrouchChangeStateEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    triggerAndExpect<CrouchChangeStateEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward},
                                            {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME / 2.f});
}

TEST_F(CharacterControllerTests, DisarmedCruchWalkState_MoveForwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    triggerAndExpect<MoveForwardEvent>({sut_.animationClipsNames_.disarmed.movement.crouch.forward});
}

TEST_F(CharacterControllerTests, DisarmedCruchWalkState_MoveLeftEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_FORWARD + VECTOR_LEFT, vec3(DEFAULT_WALK_SPEED, 0.0, DEFAULT_WALK_SPEED));
    triggerAndExpect<MoveLeftEvent>({sut_.animationClipsNames_.disarmed.movement.crouch.forward}, {0});
    expectRootboneRotation(VECTOR_FORWARD + VECTOR_LEFT);
}

TEST_F(CharacterControllerTests, DisarmedCruchWalkState_MoveRightEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_FORWARD + VECTOR_RIGHT, vec3(DEFAULT_WALK_SPEED, 0.0, DEFAULT_WALK_SPEED));
    triggerAndExpect<MoveRightEvent>({sut_.animationClipsNames_.disarmed.movement.crouch.forward}, {0});
    expectRootboneRotation(VECTOR_FORWARD + VECTOR_RIGHT);
}

TEST_F(CharacterControllerTests, DisarmedCruchWalkState_DeathEvent)
{
    prepareState(*this);
    triggerAndExpect<DeathEvent>({sut_.animationClipsNames_.disarmed.posture.crouched.death});
}

TEST_F(CharacterControllerTests, DisarmedCruchWalkState_WalkChangeStateEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    triggerAndExpect<WalkChangeStateEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
}

TEST_F(CharacterControllerTests, DisarmedCruchWalkState_WeaponStateEvent)
{
    prepareState(*this);
    triggerAndExpect<WeaponStateEvent>(
        {sut_.animationClipsNames_.armed.movement.crouch.forward, sut_.animationClipsNames_.equip.front()});
}

TEST_F(CharacterControllerTests, DisarmedCruchWalkState_EndForwardMoveEvent)
{
    prepareState(*this);
    triggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.disarmed.posture.crouched.idle});
}

TEST_F(CharacterControllerTests, DisarmedCruchWalkState_EndBackwardMoveEvent)
{
    prepareState(*this);
    expectNoMove();
    triggerAndExpect<MoveBackwardEvent>({sut_.animationClipsNames_.disarmed.posture.crouched.idle},
                                       {DEFAULT_MOVING_CHANGE_DIR_SPEED});
    expectRootboneRotation(VECTOR_FORWARD);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    triggerAndExpect<EndBackwardMoveEvent>({sut_.animationClipsNames_.disarmed.movement.crouch.forward}, {0});
    expectRootboneRotation(VECTOR_FORWARD);
}

TEST_F(CharacterControllerTests, DisarmedCruchWalkState_MoveBackwardEvent)
{
    prepareState(*this);
    expectNoMove();
    triggerAndExpect<MoveBackwardEvent>({sut_.animationClipsNames_.disarmed.posture.crouched.idle});
}

TEST_F(CharacterControllerTests, DisarmedCruchWalkState_RotateLeftEvent)
{
    prepareState(*this);
    expectRotationLeft();
    triggerAndExpect<RotateLeftEvent>({sut_.animationClipsNames_.disarmed.movement.crouch.forward});
}

TEST_F(CharacterControllerTests, DisarmedCruchWalkState_RotateRightEvent)
{
    prepareState(*this);
    expectRotationRight();
    triggerAndExpect<RotateRightEvent>({sut_.animationClipsNames_.disarmed.movement.crouch.forward});
}

TEST_F(CharacterControllerTests, DisarmedCruchWalkState_RotateTargetEvent)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    triggerAndExpect<RotateTargetEvent>(RotateTargetEvent{targetRotation.value_},
                                       {sut_.animationClipsNames_.disarmed.movement.crouch.forward});
}

TEST_F(CharacterControllerTests, DisarmedCruchWalkState_SprintStateChangeEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    triggerAndExpect<SprintStateChangeEvent>({sut_.animationClipsNames_.disarmed.sprint});
}

TEST_F(CharacterControllerTests, DisarmedCruchWalkState_MoveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedCruchWalkState_EndMoveLeftEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedCruchWalkState_EndMoveRightEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedCruchWalkState_EndRotationEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedCruchWalkState_JumpEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedCruchWalkState_JumpConfirmEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedCruchWalkState_DodgeDiveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedCruchWalkState_DodgeForwardEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedCruchWalkState_DodgeBackwardEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedCruchWalkState_DodgeLeftEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedCruchWalkState_DodgeRightEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedCruchWalkState_DodgeEndEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedCruchWalkState_AttackEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedCruchWalkState_EndAttackEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedCruchWalkState_EquipEndStateEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedCruchWalkState_DisarmEndStateEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedCruchWalkState_ReloadArrowEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedCruchWalkState_AimStartEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedCruchWalkState_AimStopEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedCruchWalkState_StartFallingEvent)
{
    prepareState(*this);
    triggerAndExpect<StartFallingEvent>({sut_.animationClipsNames_.disarmed.falling});
}

TEST_F(CharacterControllerTests, DisarmedCruchWalkState_GroundDetectionEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedCruchWalkState_HurtEvent)
{
    prepareState(*this);
    triggerAndExpect<HurtEvent>({sut_.animationClipsNames_.disarmed.posture.crouched.hurt});
}

TEST_F(CharacterControllerTests, DisarmedCruchWalkState_AnimationPlayEndEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedCruchWalkState_ChangeAnimEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedCruchWalkState_StartDialogEvent)
{
    prepareState(*this);
    expectNoMove();
    expectNoRotation();

    triggerAndExpect({sut_.animationClipsNames_.disarmed.dialog.listen}, {ADVANCED_TIME_TRANSITION_TIME},
        StartDialogEvent{.role = StartDialogEvent::Role::Listener});
}

TEST_F(CharacterControllerTests, DisarmedCruchWalkState_EndDialogEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
