#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    for (auto& state : test.sut_.animationClipsNames_.disarmed.attack)
    {
        state.stateType = GameEngine::Components::PlayStateType::run;
    }

    EXPECT_CALL(test.physicsApiMock_, GetVelocity(test.rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(test.physicsApiMock_, GetRotation(test.rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    test.expectForwardVelocity(DEFAULT_RUN_SPEED);

    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.posture.stand.idle});
    test.triggerAndExpect<MoveForwardEvent>({test.sut_.animationClipsNames_.disarmed.movement.run.forward});
    test.triggerAndExpect<AttackEvent>({test.sut_.animationClipsNames_.disarmed.attack.front().clipsSequence.front(),
                                       test.sut_.animationClipsNames_.disarmed.movement.run.forward});
}
}  // namespace

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_CrouchChangeStateEvent)
{
    prepareState(*this);
    // queue DrawArrowEvent
    triggerAndExpect<CrouchChangeStateEvent>({sut_.animationClipsNames_.disarmed.attack.front().clipsSequence.front(),
                                             sut_.animationClipsNames_.disarmed.movement.run.forward});

    // wait until attack finish
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.movement.crouch.forward});
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_DrawArrowEvent)
{
    prepareState(*this);
    // queue DrawArrowEvent
    triggerAndExpect<DrawArrowEvent>({sut_.animationClipsNames_.disarmed.attack.front().clipsSequence.front(),
                                     sut_.animationClipsNames_.disarmed.movement.run.forward});

    // wait until attack finish
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.movement.run.forward, sut_.animationClipsNames_.equip.front()});

    Update(ADVANCED_TIME_CLIP_TIME);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.movement.walk.forward, sut_.animationClipsNames_.aim.draw});
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_AimStopDuringDrawArrowEvent)
{
    prepareState(*this);
    // queue DrawArrowEvent
    triggerAndExpect<DrawArrowEvent>({sut_.animationClipsNames_.disarmed.attack.front().clipsSequence.front(),
                                     sut_.animationClipsNames_.disarmed.movement.run.forward});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    triggerAndExpect<AimStopEvent>({sut_.animationClipsNames_.disarmed.attack.front().clipsSequence.front(),
                                   sut_.animationClipsNames_.disarmed.movement.run.forward});

    // Extra check
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.movement.run.forward});
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_MoveForwardEvent)
{
    prepareState(*this);
    const auto& attackClipName = sut_.animationClipsNames_.disarmed.attack.front().clipsSequence.front();

    triggerAndExpect<EndForwardMoveEvent>({attackClipName});
    expectForwardVelocity(-DEFAULT_RUN_SPEED);
    triggerAndExpect<MoveBackwardEvent>({attackClipName, sut_.animationClipsNames_.disarmed.movement.run.forward}, {0});
    expectRootboneRotation(VECTOR_BACKWARD);
    expectNoMove();
    triggerAndExpect<MoveForwardEvent>({attackClipName});
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_MoveBackwardEvent)
{
    const auto& attackClipName = sut_.animationClipsNames_.disarmed.attack.front().clipsSequence.front();
    ;
    prepareState(*this);
    expectNoMove();
    triggerAndExpect<MoveBackwardEvent>({attackClipName});
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_MoveLeftEvent_EndMoveLeftEvent)
{
    prepareState(*this);
    const auto& attackClipName = sut_.animationClipsNames_.disarmed.attack.front().clipsSequence.front();
    ;

    expectVelocity(VECTOR_FORWARD + VECTOR_LEFT, vec3(DEFAULT_RUN_SPEED, 0.0, DEFAULT_RUN_SPEED));
    triggerAndExpect<MoveLeftEvent>({attackClipName, sut_.animationClipsNames_.disarmed.movement.run.forward}, {0});
    expectRootboneRotation(VECTOR_FORWARD + VECTOR_LEFT);
    expectVelocity(VECTOR_LEFT, vec3(DEFAULT_RUN_SPEED, 0.0, 0.0));
    triggerAndExpect<EndForwardMoveEvent>({attackClipName, sut_.animationClipsNames_.disarmed.movement.run.forward}, {0});
    expectRootboneRotation(VECTOR_LEFT);
    triggerAndExpect<EndMoveLeftEvent>({attackClipName}, {0});
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_MoveRightEvent_EndMoveRightEvent)
{
    prepareState(*this);
    const auto& attackClipName = sut_.animationClipsNames_.disarmed.attack.front().clipsSequence.front();
    ;

    expectVelocity(VECTOR_FORWARD + VECTOR_RIGHT, vec3(DEFAULT_RUN_SPEED, 0.0, DEFAULT_RUN_SPEED));
    triggerAndExpect<MoveRightEvent>({attackClipName, sut_.animationClipsNames_.disarmed.movement.run.forward}, {0});
    expectRootboneRotation(VECTOR_FORWARD + VECTOR_RIGHT);
    expectVelocity(VECTOR_RIGHT, vec3(DEFAULT_RUN_SPEED, 0.0, 0.0));
    triggerAndExpect<EndForwardMoveEvent>({attackClipName, sut_.animationClipsNames_.disarmed.movement.run.forward}, {0});
    expectRootboneRotation(VECTOR_RIGHT);
    triggerAndExpect<EndMoveRightEvent>({attackClipName}, {0});
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_DeathEvent)
{
    prepareState(*this);
    triggerAndExpect<DeathEvent>({sut_.animationClipsNames_.disarmed.posture.stand.death});
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_WalkChangeStateEvent)
{
    prepareState(*this);
    const auto& attackClipName = sut_.animationClipsNames_.disarmed.attack.front().clipsSequence.front();
    ;
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    triggerAndExpect<WalkChangeStateEvent>({attackClipName, sut_.animationClipsNames_.disarmed.movement.walk.forward});
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_WeaponStateEvent)
{
    prepareState(*this);
    const auto& attackClipName = sut_.animationClipsNames_.disarmed.attack.front().clipsSequence.front();
    ;
    triggerAndExpect<WeaponStateEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward, attackClipName});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.movement.run.forward, sut_.animationClipsNames_.equip.front()});
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_EndForwardMoveEvent)
{
    prepareState(*this);
    const auto& clip = sut_.animationClipsNames_.disarmed.attack.front().clipsSequence.front();
    ;
    expectNoMove();
    triggerAndExpect<EndForwardMoveEvent>({clip});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_EndBackwardMoveEvent)
{
    prepareState(*this);
    const auto& clipName = sut_.animationClipsNames_.disarmed.attack.front().clipsSequence.front();
    ;
    expectNoMove();
    triggerAndExpect<MoveBackwardEvent>({clipName}, {0});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    triggerAndExpect<EndBackwardMoveEvent>({clipName, sut_.animationClipsNames_.disarmed.movement.run.forward}, {0});
    triggerAndExpect<EndForwardMoveEvent>({clipName}, {0});
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_RotateLeftEvent)
{
    prepareState(*this);
    const auto& clipName = sut_.animationClipsNames_.disarmed.attack.front().clipsSequence.front();
    ;
    expectRotationLeft();
    triggerAndExpect<RotateLeftEvent>({clipName, sut_.animationClipsNames_.disarmed.movement.run.forward});
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_RotateRightEvent)
{
    prepareState(*this);
    const auto& clipName = sut_.animationClipsNames_.disarmed.attack.front().clipsSequence.front();
    ;
    expectRotationRight();
    triggerAndExpect<RotateRightEvent>({clipName, sut_.animationClipsNames_.disarmed.movement.run.forward});
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_RotateTargetEvent)
{
    prepareState(*this);
    const auto& clipName = sut_.animationClipsNames_.disarmed.attack.front().clipsSequence.front();
    ;
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    triggerAndExpect<RotateTargetEvent>(RotateTargetEvent{targetRotation.value_},
                                       {clipName, sut_.animationClipsNames_.disarmed.movement.run.forward});
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_SprintStateChangeEvent)
{
    prepareState(*this);
    const auto& clipName = sut_.animationClipsNames_.disarmed.attack.front().clipsSequence.front();
    ;
    triggerAndExpect<SprintStateChangeEvent>({clipName, sut_.animationClipsNames_.disarmed.movement.run.forward});

    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.sprint});
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_AttackEventSecondClipIsIdleOnly)
{
    prepareState(*this);
    auto& clip1                                            = sut_.animationClipsNames_.disarmed.attack[0].clipsSequence.front();
    sut_.animationClipsNames_.disarmed.attack[0].stateType = GameEngine::Components::PlayStateType::run;
    auto& clip2                                            = sut_.animationClipsNames_.disarmed.attack[1].clipsSequence.front();
    sut_.animationClipsNames_.disarmed.attack[1].stateType = GameEngine::Components::PlayStateType::idle;

    triggerAndExpect<AttackEvent>({clip1, sut_.animationClipsNames_.disarmed.movement.run.forward});

    // Wait for attack anim 1 finished, and second started
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({clip2});

    // back to run after second idleOnly clip finished
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.movement.run.forward});
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_AttackEvent)
{
    prepareState(*this);
    const auto& clip1                                      = sut_.animationClipsNames_.disarmed.attack[0].clipsSequence.front();
    auto& clip2                                            = sut_.animationClipsNames_.disarmed.attack[1].clipsSequence.front();
    const auto& clip3                                      = sut_.animationClipsNames_.disarmed.attack[2].clipsSequence.front();
    sut_.animationClipsNames_.disarmed.attack[1].stateType = GameEngine::Components::PlayStateType::idle;

    triggerAndExpect<AttackEvent>({clip1, sut_.animationClipsNames_.disarmed.movement.run.forward});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    triggerAndExpect<AttackEvent>({clip2});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({clip3, sut_.animationClipsNames_.disarmed.movement.run.forward});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.movement.run.forward});
}
TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_AttackEventWhenRunBackward)
{
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    expectForwardVelocity(-DEFAULT_RUN_SPEED);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});

    triggerAndExpect<MoveBackwardEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});

    expectNoMove();
    triggerAndExpect<AttackEvent>({sut_.animationClipsNames_.disarmed.attack.front().clipsSequence.front()});
    expectForwardVelocity(-DEFAULT_RUN_SPEED);
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.movement.run.forward});
}
TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_MoveForwardAndLeftEvent)
{
    const auto& clip = sut_.animationClipsNames_.disarmed.attack.front().clipsSequence.front();
    ;
    prepareState(*this);
    expectVelocity(VECTOR_FORWARD + VECTOR_LEFT, vec3(DEFAULT_RUN_SPEED, 0.0, DEFAULT_RUN_SPEED));
    triggerAndExpect<MoveLeftEvent>({clip, sut_.animationClipsNames_.disarmed.movement.run.forward});
    expectVelocity(VECTOR_LEFT, vec3(DEFAULT_RUN_SPEED, 0.0, 0.0));
    triggerAndExpect<EndForwardMoveEvent>({clip, sut_.animationClipsNames_.disarmed.movement.run.forward}, {0});
    triggerAndExpect<EndMoveLeftEvent>({clip}, {0});
}
TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_PR_MoveStopAndGoDuringAttack)
{
    const auto& attackClip = sut_.animationClipsNames_.disarmed.attack.front().clipsSequence.front();
    ;
    prepareState(*this);
    triggerAndExpect<EndForwardMoveEvent>({attackClip}, {0});
    triggerAndExpect<MoveForwardEvent>({attackClip, sut_.animationClipsNames_.disarmed.movement.run.forward}, {0});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    triggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.disarmed.posture.stand.idle}, {0});
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_EndMoveLeftEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_EndMoveRightEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_JumpEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_JumpConfirmEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_DodgeDiveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_DodgeForwardEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_DodgeBackwardEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_DodgeLeftEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_DodgeRightEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_DodgeEndEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_EndAttackEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_EquipEndStateEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_DisarmEndStateEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_ReloadArrowEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_AimStartEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_AimStopEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_StartFallingEvent)
{
    prepareState(*this);
    triggerAndExpect<StartFallingEvent>({sut_.animationClipsNames_.disarmed.falling});
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_GroundDetectionEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_HurtEvent)
{
    prepareState(*this);
    triggerAndExpect<HurtEvent>({sut_.animationClipsNames_.disarmed.posture.stand.hurt});
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_AnimationPlayEndEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_ChangeAnimEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_StartDialogEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_EndDialogEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_MoveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_MoveLeftEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_MoveRightEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_EndRotationEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
