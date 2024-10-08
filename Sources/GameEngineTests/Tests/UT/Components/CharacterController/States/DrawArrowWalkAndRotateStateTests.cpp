#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    EXPECT_CALL(test.inputManagerMock_, CalcualteMouseMove()).WillRepeatedly(Return(vec2i{0, 0}));
    EXPECT_CALL(test.physicsApiMock_, SetRotation(test.rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));
    EXPECT_CALL(test.physicsApiMock_, GetRotation(test.rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(test.physicsApiMock_, GetVelocity(test.rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.posture.stand.idle});
    test.tiggerAndExpect<WeaponStateEvent>(
        {test.sut_.animationClipsNames_.armed.posture.stand.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    test.tiggerAndExpect<DrawArrowEvent>({test.sut_.animationClipsNames_.aim.draw});

    test.expectForwardVelocity(DEFAULT_WALK_SPEED);
    test.tiggerAndExpect<MoveForwardEvent>(
        {test.sut_.animationClipsNames_.aim.draw, test.sut_.animationClipsNames_.armed.movement.walk.forward});
    test.tiggerAndExpect<RotateRightEvent>(
        {test.sut_.animationClipsNames_.aim.draw, test.sut_.animationClipsNames_.armed.movement.walk.forward});
}
}  // namespace

TEST_F(CharacterControllerTests, DrawArrowWalkAndRotate_CrouchChangeStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<CrouchChangeStateEvent>(
        {sut_.animationClipsNames_.aim.draw, sut_.animationClipsNames_.armed.movement.crouch.forward});
}
TEST_F(CharacterControllerTests, DrawArrowWalkAndRotate_RotateLeft)
{
    prepareState(*this);
    tiggerAndExpect<RotateLeftEvent>({sut_.animationClipsNames_.aim.draw, sut_.animationClipsNames_.armed.movement.walk.forward});
}
TEST_F(CharacterControllerTests, DrawArrowWalkAndRotate_RotateRight)
{
    prepareState(*this);
    tiggerAndExpect<RotateRightEvent>(
        {sut_.animationClipsNames_.aim.draw, sut_.animationClipsNames_.armed.movement.walk.forward});
}
TEST_F(CharacterControllerTests, DrawArrowWalkAndRotate_RotateTargetEvent)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent>(
        RotateTargetEvent{targetRotation.value_},
        {sut_.animationClipsNames_.aim.draw, sut_.animationClipsNames_.armed.movement.walk.forward});
}
TEST_F(CharacterControllerTests, DrawArrowWalkAndRotate_WeaponStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<WeaponStateEvent>(
        {sut_.animationClipsNames_.disarm, sut_.animationClipsNames_.disarmed.movement.walk.forward});

    expectRotationRight(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectRotationRight(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.movement.walk.forward});
}
TEST_F(CharacterControllerTests, DrawArrowWalkAndRotate_AimStartEvent)
{
    prepareState(*this);
    tiggerAndExpect<AimStartEvent>({sut_.animationClipsNames_.aim.idle, sut_.animationClipsNames_.armed.movement.walk.forward});
}
TEST_F(CharacterControllerTests, DrawArrowWalkAndRotate_AimStopEvent)
{
    prepareState(*this);
    tiggerAndExpect<AimStopEvent>({sut_.animationClipsNames_.armed.movement.walk.forward});
}
TEST_F(CharacterControllerTests, DrawArrowWalkAndRotate_MoveForwardEvent)
{
    prepareState(*this);
    tiggerAndExpect<MoveForwardEvent>(
        {sut_.animationClipsNames_.aim.draw, sut_.animationClipsNames_.armed.movement.walk.forward});
}
TEST_F(CharacterControllerTests, DrawArrowWalkAndRotate_MoveBackwardEvent)
{
    prepareState(*this);
    expectNoMove();
    tiggerAndExpect<MoveBackwardEvent>(
        {sut_.animationClipsNames_.aim.draw, sut_.animationClipsNames_.armed.posture.stand.rotate.right}, {0, 0});
    expectForwardVelocity(-DEFAULT_BACKWARD_WALK_SPEED);
    tiggerAndExpect<EndForwardMoveEvent>(
        {sut_.animationClipsNames_.aim.draw, sut_.animationClipsNames_.armed.movement.walk.backward});
}
TEST_F(CharacterControllerTests, DrawArrowWalkAndRotate_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent>({sut_.animationClipsNames_.armed.posture.stand.death});
}
TEST_F(CharacterControllerTests, DrawArrowWalkAndRotate_SprintStateChangeEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent>({sut_.animationClipsNames_.armed.sprint});
}
TEST_F(CharacterControllerTests, DrawArrowWalkAndRotate_WalkChangeStateEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent>(
        {sut_.animationClipsNames_.aim.draw, sut_.animationClipsNames_.armed.movement.walk.forward});
}
TEST_F(CharacterControllerTests, DrawArrowWalkAndRotate_MoveLeftEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_FORWARD + VECTOR_LEFT, vec3(DEFAULT_WALK_LEFT_RIGHT_SPEED, 0.0, DEFAULT_WALK_SPEED));
    tiggerAndExpect<MoveLeftEvent>({sut_.animationClipsNames_.aim.draw, sut_.animationClipsNames_.armed.movement.walk.forward});
}
TEST_F(CharacterControllerTests, DrawArrowWalkAndRotate_MoveRightEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_FORWARD + VECTOR_RIGHT, vec3(DEFAULT_WALK_LEFT_RIGHT_SPEED, 0.0, DEFAULT_WALK_SPEED));
    tiggerAndExpect<MoveRightEvent>({sut_.animationClipsNames_.aim.draw, sut_.animationClipsNames_.armed.movement.walk.forward});
}
TEST_F(CharacterControllerTests, DrawArrowWalkAndRotate_EndRotationEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<EndRotationEvent>(
        {sut_.animationClipsNames_.aim.draw, sut_.animationClipsNames_.armed.movement.walk.forward});
}
TEST_F(CharacterControllerTests, DrawArrowWalkAndRotate_EndForwardMoveEvent)
{
    prepareState(*this);
    tiggerAndExpect<EndForwardMoveEvent>(
        {sut_.animationClipsNames_.aim.draw, sut_.animationClipsNames_.armed.posture.stand.rotate.right});
}
TEST_F(CharacterControllerTests, DrawArrowWalkAndRotate_EndBackwardMoveEvent)
{
    prepareState(*this);
    tiggerAndExpect<EndForwardMoveEvent>(
        {sut_.animationClipsNames_.aim.draw, sut_.animationClipsNames_.armed.posture.stand.rotate.right},
        {ADVANCED_TIME_TRANSITION_TIME / 4.f});
    expectForwardVelocity(-DEFAULT_BACKWARD_WALK_SPEED);
    tiggerAndExpect<MoveBackwardEvent>(
        {sut_.animationClipsNames_.aim.draw, sut_.animationClipsNames_.armed.movement.walk.backward},
        {ADVANCED_TIME_TRANSITION_TIME});
    tiggerAndExpect<EndBackwardMoveEvent>(
        {sut_.animationClipsNames_.aim.draw, sut_.animationClipsNames_.armed.posture.stand.rotate.right},
        {ADVANCED_TIME_TRANSITION_TIME / 4.f});
}
