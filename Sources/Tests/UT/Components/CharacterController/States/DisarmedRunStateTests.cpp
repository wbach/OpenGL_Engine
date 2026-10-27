#include "../CharacterControllerTests.h"
#include "Components/Controllers/CharacterController/States/DisarmedRunState.h"
#include "Logger/Log.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    EXPECT_CALL(test.physicsApiMock_, GetVelocity(test.rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(test.physicsApiMock_, GetRotation(test.rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    test.expectForwardVelocity(DEFAULT_RUN_SPEED);

    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.posture.stand.idle});
    test.triggerAndExpect<MoveForwardEvent>({test.sut_.animationClipsNames_.disarmed.movement.run.forward});
}
}  // namespace

TEST_F(CharacterControllerTests, DisarmedRunState_CrouchChangeStateEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    triggerAndExpect<CrouchChangeStateEvent>({sut_.animationClipsNames_.disarmed.movement.crouch.forward},
                                            {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME / 2.f});
}

TEST_F(CharacterControllerTests, DisarmedRunState_DrawArrowEvent)
{
    prepareState(*this);
    triggerAndExpect<DrawArrowEvent>(
        {sut_.animationClipsNames_.armed.movement.run.forward, sut_.animationClipsNames_.equip.front()},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME / 2.f});
    Update(ADVANCED_TIME_CLIP_TIME / 2.f);

    expectForwardVelocity(DEFAULT_WALK_SPEED);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.movement.walk.forward, sut_.animationClipsNames_.aim.draw});
}

TEST_F(CharacterControllerTests, DisarmedRunState_AimStopDuringDrawArrowEvent)
{
    prepareState(*this);
    triggerAndExpect<DrawArrowEvent>(
        {sut_.animationClipsNames_.armed.movement.run.forward, sut_.animationClipsNames_.equip.front()},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME / 2.f});
    triggerAndExpect<AimStopEvent>({sut_.animationClipsNames_.armed.movement.run.forward},
                                  {ADVANCED_TIME_CLIP_TIME / 2.f, ADVANCED_TIME_TRANSITION_TIME});

    // Extra check
    Update(ADVANCED_TIME_CLIP_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.movement.run.forward});
}

TEST_F(CharacterControllerTests, DisarmedRunState_MoveForwardEvent)
{
    prepareState(*this);
    triggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    expectForwardVelocity(-DEFAULT_RUN_SPEED);
    triggerAndExpect<MoveBackwardEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward}, {0});
    expectRootboneRotation(VECTOR_BACKWARD);
    expectNoMove();
    triggerAndExpect<MoveForwardEvent>({sut_.animationClipsNames_.disarmed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, DisarmedRunState_MoveBackwardEvent)
{
    prepareState(*this);
    expectNoMove();
    triggerAndExpect<MoveBackwardEvent>({sut_.animationClipsNames_.disarmed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, DisarmedRunState_MoveLeftEvent_EndMoveLeftEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_FORWARD + VECTOR_LEFT, vec3(DEFAULT_RUN_SPEED, 0.0, DEFAULT_RUN_SPEED));
    triggerAndExpect<MoveLeftEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward}, {0});
    expectRootboneRotation(VECTOR_FORWARD + VECTOR_LEFT);
    expectVelocity(VECTOR_LEFT, vec3(DEFAULT_RUN_SPEED, 0.0, 0.0));
    triggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
    expectRootboneRotation(VECTOR_LEFT);
    triggerAndExpect<EndMoveLeftEvent>({sut_.animationClipsNames_.disarmed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, DisarmedRunState_MoveRightEvent_EndMoveRightEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_FORWARD + VECTOR_RIGHT, vec3(DEFAULT_RUN_SPEED, 0.0, DEFAULT_RUN_SPEED));
    triggerAndExpect<MoveRightEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward}, {0});
    expectRootboneRotation(VECTOR_FORWARD + VECTOR_RIGHT);
    expectVelocity(VECTOR_RIGHT, vec3(DEFAULT_RUN_SPEED, 0.0, 0.0));
    triggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
    expectRootboneRotation(VECTOR_RIGHT);
    triggerAndExpect<EndMoveRightEvent>({sut_.animationClipsNames_.disarmed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, DisarmedRunState_DeathEvent)
{
    prepareState(*this);
    triggerAndExpect<DeathEvent>({sut_.animationClipsNames_.disarmed.posture.stand.death});
}

TEST_F(CharacterControllerTests, DisarmedRunState_WalkChangeStateEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    triggerAndExpect<WalkChangeStateEvent>({sut_.animationClipsNames_.disarmed.movement.walk.forward});
}

TEST_F(CharacterControllerTests, DisarmedRunState_WeaponStateEvent)
{
    prepareState(*this);
    triggerAndExpect<WeaponStateEvent>(
        {sut_.animationClipsNames_.armed.movement.run.forward, sut_.animationClipsNames_.equip.front()});
}

TEST_F(CharacterControllerTests, DisarmedRunState_EquipAnimNotSetWeaponStateEvent)
{
    prepareState(*this);
    sut_.animationClipsNames_.equip.clear();
    triggerAndExpect<WeaponStateEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});

    EXPECT_EQ(sut_.getCurrentStateName(), typeName<DisarmedRunState>());
}

TEST_F(CharacterControllerTests, DisarmedRunState_EndForwardMoveEvent)
{
    prepareState(*this);
    triggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.disarmed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, DisarmedRunState_EndBackwardMoveEvent)
{
    prepareState(*this);
    expectNoMove();
    triggerAndExpect<MoveBackwardEvent>({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    triggerAndExpect<EndBackwardMoveEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
    triggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.disarmed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, DisarmedRunState_RotateLeftEvent)
{
    prepareState(*this);
    expectRotationLeft();
    triggerAndExpect<RotateLeftEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
}

TEST_F(CharacterControllerTests, DisarmedRunState_RotateRightEvent)
{
    prepareState(*this);
    expectRotationRight();
    triggerAndExpect<RotateRightEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
}

TEST_F(CharacterControllerTests, DisarmedRunState_RotateTargetEvent)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    triggerAndExpect<RotateTargetEvent>(RotateTargetEvent{targetRotation.value_},
                                       {sut_.animationClipsNames_.disarmed.movement.run.forward});
}

TEST_F(CharacterControllerTests, DisarmedRunState_SprintStateChangeEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    triggerAndExpect<SprintStateChangeEvent>({sut_.animationClipsNames_.disarmed.sprint});
}

TEST_F(CharacterControllerTests, DisarmedRunState_UpdateVelocityInNewStateWhenTransitionIsNotFinishedInPreviousOne)
{
    prepareState(*this);
    float deltaTime = {0.0001f};
    expectRotationLeft(deltaTime);
    triggerAndExpect<RotateLeftEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward}, {deltaTime});
    // velocity should be updated in new state too like in disarmed run state
    expectRotationLeft();
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    Update(ADVANCED_TIME_TRANSITION_TIME);
}
TEST_F(CharacterControllerTests, DisarmedRunState_AttackEvent)
{
    prepareState(*this);
    expectNoMove();
    triggerAndExpect<AttackEvent>({sut_.animationClipsNames_.disarmed.attack.front().clipsSequence.front()});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.movement.run.forward});
}

TEST_F(CharacterControllerTests, DisarmedRunState_AttackAnimationNotSetWhenAttackEventIncoming)
{
    prepareState(*this);
    sut_.animationClipsNames_.disarmed.attack.clear();
    triggerAndExpect<AttackEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
    EXPECT_EQ(sut_.getCurrentStateName(), typeName<DisarmedRunState>());
}

TEST_F(CharacterControllerTests, DisarmedRunState_AttackEventWhenRunBackward)
{
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    expectForwardVelocity(-DEFAULT_RUN_SPEED);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});

    triggerAndExpect<MoveBackwardEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});

    expectNoMove();
    triggerAndExpect<AttackEvent>({sut_.animationClipsNames_.disarmed.attack.front().clipsSequence.front()});
    expectForwardVelocity(-DEFAULT_RUN_SPEED);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.movement.run.forward});
}

TEST_F(CharacterControllerTests, DisarmedRunState_MoveEvent)
{
    prepareState(*this);
    expectIgnoreEvent<MoveEvent>();
}

TEST_F(CharacterControllerTests, DisarmedRunState_JumpConfirmEvent)
{
    prepareState(*this);
    expectIgnoreEvent<JumpConfirmEvent>();
}

TEST_F(CharacterControllerTests, DisarmedRunState_DodgeDiveEvent)
{
    prepareState(*this);
    triggerAndExpect<DodgeDiveEvent>({sut_.animationClipsNames_.disarmed.dodgeDive});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.movement.run.forward});
}

TEST_F(CharacterControllerTests, DisarmedRunState_DodgeForwardEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedRunState_DodgeBackwardEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedRunState_DodgeLeftEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedRunState_DodgeRightEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedRunState_DodgeEndEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedRunState_EndAttackEvent)
{
    prepareState(*this);
    expectIgnoreEvent<EndAttackEvent>();
}

TEST_F(CharacterControllerTests, DisarmedRunState_EquipEndStateEvent)
{
    prepareState(*this);
    expectIgnoreEvent<EquipEndStateEvent>();
}

TEST_F(CharacterControllerTests, DisarmedRunState_DisarmEndStateEvent)
{
    prepareState(*this);
    expectIgnoreEvent<DisarmEndStateEvent>();
}

TEST_F(CharacterControllerTests, DisarmedRunState_ReloadArrowEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedRunState_AimStartEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedRunState_StartFallingEvent)
{
    prepareState(*this);
    triggerAndExpect<StartFallingEvent>({sut_.animationClipsNames_.disarmed.falling});
}

TEST_F(CharacterControllerTests, DisarmedRunState_GroundDetectionEvent)
{
    prepareState(*this);
    expectIgnoreEvent<GroundDetectionEvent>();
}

TEST_F(CharacterControllerTests, DisarmedRunState_HurtEvent)
{
    prepareState(*this);
    triggerAndExpect<HurtEvent>({sut_.animationClipsNames_.disarmed.posture.stand.hurt});
}

TEST_F(CharacterControllerTests, DisarmedRunState_AnimationPlayEndEvent)
{
    prepareState(*this);
    expectIgnoreEvent<AnimationPlayEndEvent>();
}

TEST_F(CharacterControllerTests, DisarmedRunState_ChangeAnimEvent)
{
    prepareState(*this);
    expectIgnoreEvent<ChangeAnimEvent>();
}

TEST_F(CharacterControllerTests, DisarmedRunState_StartDialogEvent)
{
    prepareState(*this);
    expectNoMove();
    expectNoRotation();
    triggerAndExpect({sut_.animationClipsNames_.disarmed.dialog.listen}, {ADVANCED_TIME_TRANSITION_TIME},
                    StartDialogEvent{.role = StartDialogEvent::Role::Listener});
    triggerAndExpect<EndDialogEvent>({sut_.animationClipsNames_.disarmed.posture.stand.idle}, {ADVANCED_TIME_TRANSITION_TIME});
}

TEST_F(CharacterControllerTests, DisarmedRunState_EndDialogEvent)
{
    prepareState(*this);
    expectIgnoreEvent<EndDialogEvent>();
}

TEST_F(CharacterControllerTests, DisarmedRunState_JumpEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedRunState_EndRotationEvent)
{
    prepareState(*this);
    expectIgnoreEvent<EndRotationEvent>();
}

TEST_F(CharacterControllerTests, DisarmedRunState_AimStopEvent)
{
    prepareState(*this);
    expectIgnoreEvent<AimStopEvent>();
}

TEST_F(CharacterControllerTests, DisarmedRunState_EndMoveEventWhenAttackEventIsProcessing)
{
    prepareState(*this);
    expectNoMove();
    triggerAndExpect<AttackEvent>({sut_.animationClipsNames_.disarmed.attack.front().clipsSequence.front()});
    triggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.disarmed.attack.front().clipsSequence.front()});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
}
TEST_F(CharacterControllerTests, DISABLED_DisarmedRunState_JumpEvent)
{
    prepareState(*this);
    const float jumpPower{1.f};
    LOG_DEBUG << "lastSetVelocity=" << lastSetVelocity;

    // expectVelocity(VECTOR_FORWARD + VECTOR_UP, vec3(0, jumpPower, DEFAULT_RUN_SPEED), lastSetVelocity);
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(lastSetVelocity));
    EXPECT_CALL(physicsApiMock_, SetVelocityRigidbody(rigidbodyid, lastSetVelocity + vec3(0, jumpPower, 0)))
        .Times(AtLeast(1))
        .WillRepeatedly(SaveArg<1>(&lastSetVelocity));

    EXPECT_CALL(physicsApiMock_, RayTest(_, _)).WillRepeatedly(Return(std::nullopt));

    triggerAndExpect<JumpEvent>({sut_.animationClipsNames_.disarmed.jump}, {ADVANCED_TIME_TRANSITION_TIME});

    Update(ADVANCED_TIME_CLIP_TIME);

    {
        GameEngine::Physics::RayHit rayHit{
            .gameObject = *obj_, .pointWorld = vec3(0, -1, 0), .normalWorld = vec3(0, -1, 0), .rigidbodyId = rigidbodyid.value()};
        EXPECT_CALL(physicsApiMock_, RayTest(_, _)).WillOnce(Return(rayHit));
        Update(ADVANCED_TIME_TRANSITION_TIME);
    }
    {
        GameEngine::Physics::RayHit rayHit{
            .gameObject = *obj_, .pointWorld = vec3(0, 0, 0), .normalWorld = vec3(0, -1, 0), .rigidbodyId = rigidbodyid.value()};
        EXPECT_CALL(physicsApiMock_, RayTest(_, _)).WillOnce(Return(rayHit));
        Update(ADVANCED_TIME_TRANSITION_TIME);
    }

    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.movement.run.forward});
}
