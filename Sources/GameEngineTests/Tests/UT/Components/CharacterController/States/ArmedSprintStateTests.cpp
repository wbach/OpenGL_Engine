#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    EXPECT_CALL(test.physicsApiMock_, GetVelocity(test.rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(test.physicsApiMock_, GetRotation(test.rigidbodyid)).WillRepeatedly(Return(Rotation().value_));

    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.idle});
    test.tiggerAndExpect<WeaponStateEvent>(
        {test.sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
    test.expectForwardVelocity(DEFAULT_RUN_SPEED);
    test.tiggerAndExpect<MoveForwardEvent>({test.sut_.animationClipsNames_.armed.run.forward});
    test.expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    test.tiggerAndExpect<SprintStateChangeEvent>({test.sut_.animationClipsNames_.armed.sprint});
}
}  // namespace

TEST_F(CharacterControllerTests, ArmedSprintState_CrouchChangeStateEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<CrouchChangeStateEvent>({sut_.animationClipsNames_.armed.crouch.forward});
}
TEST_F(CharacterControllerTests, ArmedSprintState_DrawArrowEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<DrawArrowEvent>(
        {sut_.animationClipsNames_.armed.walk.forward, sut_.animationClipsNames_.drawArrow});
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
    tiggerAndExpect<MoveBackwardEvent>({sut_.animationClipsNames_.armed.idle});
}
TEST_F(CharacterControllerTests, ArmedSprintState_EndBackwardMoveEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<WeaponStateEvent>(
        {sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
    expectForwardVelocity(-DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveBackwardEvent>({sut_.animationClipsNames_.armed.run.forward});
    expectRootboneRotation(VECTOR_BACKWARD);
    expectForwardVelocity(-DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent>({sut_.animationClipsNames_.armed.sprint});
    expectRootboneRotation(VECTOR_BACKWARD);
    tiggerAndExpect<EndBackwardMoveEvent>({sut_.animationClipsNames_.armed.idle});
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
    tiggerAndExpect<DeathEvent>({sut_.animationClipsNames_.armed.death});
}
TEST_F(CharacterControllerTests, ArmedSprintState_WalkChangeStateEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent>({sut_.animationClipsNames_.armed.walk.forward});
}
TEST_F(CharacterControllerTests, ArmedSprintState_SprintStateChangeEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<SprintStateChangeEvent>({sut_.animationClipsNames_.armed.run.forward});
}
TEST_F(CharacterControllerTests, ArmedSprintState_WeaponStateEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<WeaponStateEvent>(
        {sut_.animationClipsNames_.disarmed.run.forward},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
}
TEST_F(CharacterControllerTests, ArmedSprintState_EndForwardMoveEvent)
{
    prepareState(*this);
    tiggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.armed.idle});
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
    tiggerAndExpect<RotateTargetEvent>(RotateTargetEvent{targetRotation.value_},
                                       {sut_.animationClipsNames_.armed.sprint});
}
