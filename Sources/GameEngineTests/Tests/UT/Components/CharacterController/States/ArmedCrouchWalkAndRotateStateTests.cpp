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
    test.expectForwardVelocity(DEFAULT_WALK_SPEED);
    test.tiggerAndExpect<WalkChangeStateEvent>({test.sut_.animationClipsNames_.armed.movement.walk.forward});
    test.expectRotationRight();
    test.tiggerAndExpect<RotateRightEvent>({test.sut_.animationClipsNames_.armed.movement.walk.forward});
    test.tiggerAndExpect<CrouchChangeStateEvent>({test.sut_.animationClipsNames_.armed.movement.crouch.forward});
}
}  // namespace

TEST_F(CharacterControllerTests, ArmedCrouchWalkAndRotate_CrouchChangeStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<CrouchChangeStateEvent>({sut_.animationClipsNames_.armed.movement.run.forward});
}

TEST_F(CharacterControllerTests, ArmedCrouchWalkAndRotate_DrawArrowEvent)
{
    prepareState(*this);
    tiggerAndExpect<DrawArrowEvent>(
        {sut_.animationClipsNames_.aim.draw, sut_.animationClipsNames_.armed.movement.crouch.forward});
}

TEST_F(CharacterControllerTests, ArmedCrouchWalkAndRotate_RotateLeftEvent)
{
    prepareState(*this);
    expectRotationLeft();
    tiggerAndExpect<RotateLeftEvent>({sut_.animationClipsNames_.armed.movement.crouch.forward});
}

TEST_F(CharacterControllerTests, ArmedCrouchWalkAndRotate_RotateRightEvent)
{
    prepareState(*this);
    expectRotationLeft();
    tiggerAndExpect<RotateLeftEvent>({sut_.animationClipsNames_.armed.movement.crouch.forward});
    expectRotationRight();
    tiggerAndExpect<RotateRightEvent>({sut_.animationClipsNames_.armed.movement.crouch.forward});
}

TEST_F(CharacterControllerTests, ArmedCrouchWalkAndRotate_RotateTargetEvent)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent>(RotateTargetEvent{targetRotation.value_},
                                       {sut_.animationClipsNames_.armed.movement.crouch.forward});
}

TEST_F(CharacterControllerTests, ArmedCrouchWalkAndRotate_MoveForwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<MoveForwardEvent>({sut_.animationClipsNames_.armed.movement.crouch.forward});
}

TEST_F(CharacterControllerTests, ArmedCrouchWalkAndRotate_MoveBackwardEvent)
{
    prepareState(*this);
    expectNoMove();
    tiggerAndExpect<MoveBackwardEvent>({sut_.animationClipsNames_.armed.posture.crouched.rotate.right});
    expectForwardVelocity(-DEFAULT_WALK_SPEED);
    tiggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.armed.movement.crouch.forward});
    expectRotationRight(DEFAULT_MOVING_CHANGE_DIR_SPEED);
    expectRootboneRotation(VECTOR_BACKWARD);
}

TEST_F(CharacterControllerTests, ArmedCrouchWalkAndRotate_MoveLeftEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_FORWARD + VECTOR_LEFT, vec3(DEFAULT_WALK_SPEED, 0.0, DEFAULT_WALK_SPEED));
    tiggerAndExpect<MoveLeftEvent>({sut_.animationClipsNames_.armed.movement.crouch.forward});
    expectRotationRight(DEFAULT_MOVING_CHANGE_DIR_SPEED);
    expectRootboneRotation(VECTOR_FORWARD + VECTOR_LEFT);
}

TEST_F(CharacterControllerTests, ArmedCrouchWalkAndRotate_MoveRightEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_FORWARD + VECTOR_RIGHT, vec3(DEFAULT_WALK_SPEED, 0.0, DEFAULT_WALK_SPEED));
    tiggerAndExpect<MoveRightEvent>({sut_.animationClipsNames_.armed.movement.crouch.forward});
    expectRotationRight(DEFAULT_MOVING_CHANGE_DIR_SPEED);
    expectRootboneRotation(VECTOR_FORWARD + VECTOR_RIGHT);
}

TEST_F(CharacterControllerTests, ArmedCrouchWalkAndRotate_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent>({sut_.animationClipsNames_.armed.posture.crouched.death});
}

TEST_F(CharacterControllerTests, ArmedCrouchWalkAndRotate_WalkChangeStateEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<WalkChangeStateEvent>({sut_.animationClipsNames_.armed.movement.run.forward});
}

TEST_F(CharacterControllerTests, ArmedCrouchWalkAndRotate_WeaponStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<WeaponStateEvent>(
        {sut_.animationClipsNames_.disarmed.movement.crouch.forward, sut_.animationClipsNames_.disarm});
}

TEST_F(CharacterControllerTests, ArmedCrouchWalkAndRotate_EndForwardMoveEvent)
{
    prepareState(*this);
    tiggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.armed.posture.stand.rotate.right});
}

TEST_F(CharacterControllerTests, ArmedCrouchWalkAndRotate_EndBackwardMoveEvent)
{
    prepareState(*this);
    expectNoMove();
    tiggerAndExpect<MoveBackwardEvent>({sut_.animationClipsNames_.armed.posture.stand.rotate.right});
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<EndBackwardMoveEvent>({sut_.animationClipsNames_.armed.movement.crouch.forward});
}

TEST_F(CharacterControllerTests, ArmedCrouchWalkAndRotate_EndRotationEvent)
{
    prepareState(*this);
    tiggerAndExpect<EndRotationEvent>({sut_.animationClipsNames_.armed.movement.crouch.forward});
}

TEST_F(CharacterControllerTests, ArmedCrouchWalkAndRotate_SprintStateChangeEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent>({sut_.animationClipsNames_.armed.sprint});
}

TEST_F(CharacterControllerTests, ArmedCrouchWalkAndRotate_SprintStartEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent>({sut_.animationClipsNames_.armed.sprint});
}
