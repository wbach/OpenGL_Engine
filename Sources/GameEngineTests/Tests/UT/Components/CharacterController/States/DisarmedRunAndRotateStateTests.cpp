#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    EXPECT_CALL(test.physicsApiMock_, GetVelocity(test.rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(test.physicsApiMock_, GetRotation(test.rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    test.expectForwardVelocity(DEFAULT_RUN_SPEED);
    test.expectRotationLeft();

    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.posture.stand.idle});
    test.tiggerAndExpect<MoveForwardEvent>({test.sut_.animationClipsNames_.disarmed.movement.run.forward});
    test.tiggerAndExpect<RotateLeftEvent>({test.sut_.animationClipsNames_.disarmed.movement.run.forward});
}
}  // namespace

TEST_F(CharacterControllerTests, DisarmedRunAndRotateState_CrouchChangeStateEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<CrouchChangeStateEvent>({sut_.animationClipsNames_.disarmed.movement.crouch.forward});
}
TEST_F(CharacterControllerTests, DisarmedRunAndRotateState_DrawArrowEvent)
{
    prepareState(*this);
    tiggerAndExpect<DrawArrowEvent>({sut_.animationClipsNames_.armed.movement.run.forward, sut_.animationClipsNames_.equip},
                                    {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_TRANSITION_TIME});
    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectRotationLeft();

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectForwardVelocity(DEFAULT_WALK_SPEED);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.movement.walk.forward, sut_.animationClipsNames_.aim.draw});
}

TEST_F(CharacterControllerTests, DisarmedRunAndRotateState_AimStopDuringDrawArrowEvent)
{
    prepareState(*this);
    tiggerAndExpect<DrawArrowEvent>({sut_.animationClipsNames_.armed.movement.run.forward, sut_.animationClipsNames_.equip},
                                    {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_TRANSITION_TIME});
    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    tiggerAndExpect<AimStopEvent>({sut_.animationClipsNames_.armed.movement.run.forward},
                                  {ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_CLIP_TIME});

    // Extra check
    Update(ADVANCED_TIME_CLIP_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.movement.run.forward});
}

TEST_F(CharacterControllerTests, DisarmedRunAndRotateState_MoveForwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveForwardEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
}

TEST_F(CharacterControllerTests, DisarmedRunAndRotateState_MoveBackwardEvent)
{
    prepareState(*this);
    expectNoMove();
    tiggerAndExpect<MoveBackwardEvent>({sut_.animationClipsNames_.disarmed.posture.stand.rotate.left});
    expectForwardVelocity(-DEFAULT_RUN_SPEED);
    tiggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
    expectRotationLeft(DEFAULT_MOVING_CHANGE_DIR_SPEED);
    expectRootboneRotation(VECTOR_BACKWARD);
}

TEST_F(CharacterControllerTests, DisarmedRunAndRotateState_MoveLeftEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_FORWARD + VECTOR_LEFT, vec3(DEFAULT_RUN_SPEED, 0.0, DEFAULT_RUN_SPEED));
    tiggerAndExpect<MoveLeftEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
    expectRotationLeft(DEFAULT_MOVING_CHANGE_DIR_SPEED);
    expectRootboneRotation(VECTOR_FORWARD + VECTOR_LEFT);
}

TEST_F(CharacterControllerTests, DisarmedRunAndRotateState_MoveRightEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_FORWARD + VECTOR_RIGHT, vec3(DEFAULT_RUN_SPEED, 0.0, DEFAULT_RUN_SPEED));
    tiggerAndExpect<MoveRightEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
    expectRotationLeft(DEFAULT_MOVING_CHANGE_DIR_SPEED);
    expectRootboneRotation(VECTOR_FORWARD + VECTOR_RIGHT);
}

TEST_F(CharacterControllerTests, DisarmedRunAndRotateState_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent>({sut_.animationClipsNames_.disarmed.posture.stand.death});
}

TEST_F(CharacterControllerTests, DisarmedRunAndRotateState_WalkChangeStateEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent>({sut_.animationClipsNames_.disarmed.movement.walk.forward});
}

TEST_F(CharacterControllerTests, DisarmedRunAndRotateState_WeaponStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<WeaponStateEvent>({sut_.animationClipsNames_.armed.movement.run.forward, sut_.animationClipsNames_.equip});
}

TEST_F(CharacterControllerTests, DisarmedRunAndRotateState_EndForwardMoveEvent)
{
    prepareState(*this);
    tiggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.disarmed.posture.stand.rotate.left});
}

TEST_F(CharacterControllerTests, DisarmedRunAndRotateState_EndBackwardMoveEvent)
{
    prepareState(*this);
    expectNoMove();
    tiggerAndExpect<MoveBackwardEvent>({sut_.animationClipsNames_.disarmed.posture.stand.rotate.left});
    expectForwardVelocity(-DEFAULT_RUN_SPEED);
    tiggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
    expectRotationLeft(DEFAULT_MOVING_CHANGE_DIR_SPEED);
    expectRootboneRotation(VECTOR_BACKWARD);
    tiggerAndExpect<EndBackwardMoveEvent>({sut_.animationClipsNames_.disarmed.posture.stand.rotate.left});
}

TEST_F(CharacterControllerTests, DisarmedRunAndRotateState_RotateLeftEvent)
{
    prepareState(*this);
    expectRotationLeft();
    tiggerAndExpect<RotateLeftEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
}

TEST_F(CharacterControllerTests, DisarmedRunAndRotateState_RotateRightEvent)
{
    prepareState(*this);
    expectRotationRight();
    tiggerAndExpect<RotateRightEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
}

TEST_F(CharacterControllerTests, DisarmedRunAndRotateState_RotateTargetEvent)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent>(RotateTargetEvent{targetRotation.value_},
                                       {sut_.animationClipsNames_.disarmed.movement.run.forward});
}

TEST_F(CharacterControllerTests, DisarmedRunAndRotateState_SprintStateChangeEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent>({sut_.animationClipsNames_.disarmed.sprint});
}

TEST_F(CharacterControllerTests, DisarmedRunAndRotateState_EndRotationEvent)
{
    prepareState(*this);
    tiggerAndExpect<EndRotationEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
}
