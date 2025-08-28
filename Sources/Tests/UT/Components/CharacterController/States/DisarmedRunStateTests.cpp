#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    EXPECT_CALL(test.physicsApiMock_, GetVelocity(test.rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(test.physicsApiMock_, GetRotation(test.rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    test.expectForwardVelocity(DEFAULT_RUN_SPEED);

    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.posture.stand.idle});
    test.tiggerAndExpect<MoveForwardEvent>({test.sut_.animationClipsNames_.disarmed.movement.run.forward});
}
}  // namespace

TEST_F(CharacterControllerTests, DisarmedRunState_CrouchChangeStateEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<CrouchChangeStateEvent>({sut_.animationClipsNames_.disarmed.movement.crouch.forward},
                                            {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME / 2.f});
}

TEST_F(CharacterControllerTests, DisarmedRunState_DrawArrowEvent)
{
    prepareState(*this);
    tiggerAndExpect<DrawArrowEvent>({sut_.animationClipsNames_.armed.movement.run.forward, sut_.animationClipsNames_.equip},
                                    {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME / 2.f});
    Update(ADVANCED_TIME_CLIP_TIME / 2.f);

    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.movement.walk.forward, sut_.animationClipsNames_.aim.draw});
}

TEST_F(CharacterControllerTests, DisarmedRunState_AimStopDuringDrawArrowEvent)
{
    prepareState(*this);
    tiggerAndExpect<DrawArrowEvent>({sut_.animationClipsNames_.armed.movement.run.forward, sut_.animationClipsNames_.equip},
                                    {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME / 2.f});
    tiggerAndExpect<AimStopEvent>({sut_.animationClipsNames_.armed.movement.run.forward},
                                  {ADVANCED_TIME_CLIP_TIME / 2.f, ADVANCED_TIME_TRANSITION_TIME});

    // Extra check
    Update(ADVANCED_TIME_CLIP_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.movement.run.forward});
}

TEST_F(CharacterControllerTests, DisarmedRunState_MoveForwardEvent)
{
    prepareState(*this);
    tiggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    expectForwardVelocity(-DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveBackwardEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward}, {0});
    expectRootboneRotation(VECTOR_BACKWARD);
    expectNoMove();
    tiggerAndExpect<MoveForwardEvent>({sut_.animationClipsNames_.disarmed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, DisarmedRunState_MoveBackwardEvent)
{
    prepareState(*this);
    expectNoMove();
    tiggerAndExpect<MoveBackwardEvent>({sut_.animationClipsNames_.disarmed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, DisarmedRunState_MoveLeftEvent_EndMoveLeftEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_FORWARD + VECTOR_LEFT, vec3(DEFAULT_RUN_SPEED, 0.0, DEFAULT_RUN_SPEED));
    tiggerAndExpect<MoveLeftEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward}, {0});
    expectRootboneRotation(VECTOR_FORWARD + VECTOR_LEFT);
    expectVelocity(VECTOR_LEFT, vec3(DEFAULT_RUN_SPEED, 0.0, 0.0));
    tiggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
    expectRootboneRotation(VECTOR_LEFT);
    tiggerAndExpect<EndMoveLeftEvent>({sut_.animationClipsNames_.disarmed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, DisarmedRunState_MoveRightEvent_EndMoveRightEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_FORWARD + VECTOR_RIGHT, vec3(DEFAULT_RUN_SPEED, 0.0, DEFAULT_RUN_SPEED));
    tiggerAndExpect<MoveRightEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward}, {0});
    expectRootboneRotation(VECTOR_FORWARD + VECTOR_RIGHT);
    expectVelocity(VECTOR_RIGHT, vec3(DEFAULT_RUN_SPEED, 0.0, 0.0));
    tiggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
    expectRootboneRotation(VECTOR_RIGHT);
    tiggerAndExpect<EndMoveRightEvent>({sut_.animationClipsNames_.disarmed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, DisarmedRunState_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent>({sut_.animationClipsNames_.disarmed.posture.stand.death});
}

TEST_F(CharacterControllerTests, DisarmedRunState_WalkChangeStateEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent>({sut_.animationClipsNames_.disarmed.movement.walk.forward});
}

TEST_F(CharacterControllerTests, DisarmedRunState_WeaponStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<WeaponStateEvent>({sut_.animationClipsNames_.armed.movement.run.forward, sut_.animationClipsNames_.equip});
}

TEST_F(CharacterControllerTests, DisarmedRunState_EndForwardMoveEvent)
{
    prepareState(*this);
    tiggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.disarmed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, DisarmedRunState_EndBackwardMoveEvent)
{
    prepareState(*this);
    expectNoMove();
    tiggerAndExpect<MoveBackwardEvent>({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<EndBackwardMoveEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
    tiggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.disarmed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, DisarmedRunState_RotateLeftEvent)
{
    prepareState(*this);
    expectRotationLeft();
    tiggerAndExpect<RotateLeftEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
}

TEST_F(CharacterControllerTests, DisarmedRunState_RotateRightEvent)
{
    prepareState(*this);
    expectRotationRight();
    tiggerAndExpect<RotateRightEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
}

TEST_F(CharacterControllerTests, DisarmedRunState_RotateTargetEvent)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent>(RotateTargetEvent{targetRotation.value_},
                                       {sut_.animationClipsNames_.disarmed.movement.run.forward});
}

TEST_F(CharacterControllerTests, DisarmedRunState_SprintStateChangeEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent>({sut_.animationClipsNames_.disarmed.sprint});
}

TEST_F(CharacterControllerTests, DisarmedRunState_UpdateVelocityInNewStateWhenTransitionIsNotFinishedInPreviousOne)
{
    prepareState(*this);
    float deltaTime = {0.0001f};
    expectRotationLeft(deltaTime);
    tiggerAndExpect<RotateLeftEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward}, {deltaTime});
    // velocity should be updated in new state too like in disarmed run state
    expectRotationLeft();
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    Update(ADVANCED_TIME_TRANSITION_TIME);
}
TEST_F(CharacterControllerTests, DisarmedRunState_AttackEvent)
{
    prepareState(*this);
    expectNoMove();
    tiggerAndExpect<AttackEvent>({sut_.animationClipsNames_.disarmed.attack.front().name});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.movement.run.forward});
}
TEST_F(CharacterControllerTests, DisarmedRunState_AttackEventWhenRunBackward)
{
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    expectForwardVelocity(-DEFAULT_RUN_SPEED);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});

    tiggerAndExpect<MoveBackwardEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});

    expectNoMove();
    tiggerAndExpect<AttackEvent>({sut_.animationClipsNames_.disarmed.attack.front().name});
    expectForwardVelocity(-DEFAULT_RUN_SPEED);
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.movement.run.forward});
}
TEST_F(CharacterControllerTests, DisarmedRunState_EndMoveEventWhenAttackEventIsProcessing)
{
    prepareState(*this);
    expectNoMove();
    tiggerAndExpect<AttackEvent>({sut_.animationClipsNames_.disarmed.attack.front().name});
    tiggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.disarmed.attack.front().name});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
}
TEST_F(CharacterControllerTests, DISABLED_DisarmedRunState_JumpEvent)
{
    prepareState(*this);
    const float jumpPower{1.f};
    DEBUG_LOG("lastSetVelocity=" + std::to_string(lastSetVelocity));

    // expectVelocity(VECTOR_FORWARD + VECTOR_UP, vec3(0, jumpPower, DEFAULT_RUN_SPEED), lastSetVelocity);
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(lastSetVelocity));
    EXPECT_CALL(physicsApiMock_, SetVelocityRigidbody(rigidbodyid, lastSetVelocity + vec3(0, jumpPower, 0)))
        .Times(AtLeast(1))
        .WillRepeatedly(SaveArg<1>(&lastSetVelocity));

    EXPECT_CALL(physicsApiMock_, RayTest(_, _)).WillRepeatedly(Return(std::nullopt));

    tiggerAndExpect<JumpEvent>({sut_.animationClipsNames_.disarmed.jump}, {ADVANCED_TIME_TRANSITION_TIME});

    Update(ADVANCED_TIME_CLIP_TIME);

    {
        GameEngine::Physics::RayHit rayHit{
            .pointWorld = vec3(0, -1, 0), .normalWorld = vec3(0, -1, 0), .rigidbodyId = rigidbodyid.value()};
        EXPECT_CALL(physicsApiMock_, RayTest(_, _)).WillOnce(Return(rayHit));
        Update(ADVANCED_TIME_TRANSITION_TIME);
    }
    {
        GameEngine::Physics::RayHit rayHit{
            .pointWorld = vec3(0, 0, 0), .normalWorld = vec3(0, -1, 0), .rigidbodyId = rigidbodyid.value()};
        EXPECT_CALL(physicsApiMock_, RayTest(_, _)).WillOnce(Return(rayHit));
        Update(ADVANCED_TIME_TRANSITION_TIME);
    }

    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.movement.run.forward});
}
