#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    EXPECT_CALL(test.physicsApiMock_, GetVelocity(test.rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(test.physicsApiMock_, GetRotation(test.rigidbodyid)).WillRepeatedly(Return(Rotation().value_));

    test.expectState<DisarmedIdleState>();
    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.idle});
    test.tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {test.sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
    test.expectRotationLeft();
    test.tiggerAndExpect<RotateLeftEvent, ArmedRotateState>({test.sut_.animationClipsNames_.armed.rotateLeft});
    test.expectForwardVelocity(DEFAULT_RUN_SPEED);
    test.tiggerAndExpect<MoveForwardEvent, ArmedSprintAndRotateState>({test.sut_.animationClipsNames_.armed.run.forward});
    test.expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    test.tiggerAndExpect<SprintStateChangeEvent, ArmedSprintAndRotateState>({test.sut_.animationClipsNames_.armed.sprint});
}
}  // namespace

TEST_F(CharacterControllerTests, ArmedSprintAndRotateState_DrawArrowEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<DrawArrowEvent, DrawArrowWalkAndRotateState>(
        {sut_.animationClipsNames_.armed.walk.forward, sut_.animationClipsNames_.drawArrow});
}
TEST_F(CharacterControllerTests, ArmedSprintAndRotateState_MoveForwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveForwardEvent, ArmedRunAndRotateState>({sut_.animationClipsNames_.armed.run.forward});
}
TEST_F(CharacterControllerTests, ArmedSprintAndRotateState_MoveBackwardEvent)
{
    prepareState(*this);
    expectNoMove();
    tiggerAndExpect<MoveBackwardEvent, ArmedRunAndRotateState>({sut_.animationClipsNames_.armed.rotateLeft});
    expectForwardVelocity(-DEFAULT_RUN_SPEED);
    tiggerAndExpect<EndForwardMoveEvent, ArmedRunAndRotateState>({sut_.animationClipsNames_.armed.run.forward});
    expectRootboneRotation(VECTOR_BACKWARD);
}
TEST_F(CharacterControllerTests, ArmedSprintAndRotateState_MoveLeftEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_LEFT, vec3(DEFAULT_SPRINT_SPEED, 0.0, 0));
    tiggerAndExpect<MoveLeftEvent, ArmedRunAndRotateState>({sut_.animationClipsNames_.armed.sprint});
    expectRootboneRotation(VECTOR_LEFT);
}

TEST_F(CharacterControllerTests, ArmedSprintAndRotateState_MoveRightEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_RIGHT, vec3(DEFAULT_SPRINT_SPEED, 0.0, 0));
    tiggerAndExpect<MoveRightEvent, ArmedRunAndRotateState>({sut_.animationClipsNames_.armed.sprint});
    expectRootboneRotation(VECTOR_RIGHT);
}
TEST_F(CharacterControllerTests, ArmedSprintAndRotateState_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent, DeathState>({sut_.animationClipsNames_.armed.death});
}
TEST_F(CharacterControllerTests, ArmedSprintAndRotateState_WalkChangeStateEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent, ArmedWalkAndRotateState>({sut_.animationClipsNames_.armed.walk.forward});
}
TEST_F(CharacterControllerTests, ArmedSprintAndRotateState_SprintStateChangeEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<SprintStateChangeEvent, ArmedRunAndRotateState>({sut_.animationClipsNames_.armed.run.forward});
}
TEST_F(CharacterControllerTests, ArmedSprintAndRotateState_WeaponStateEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<WeaponStateEvent, RunAndRotateArmedChangeState>(
        {sut_.animationClipsNames_.disarmed.run.forward, sut_.animationClipsNames_.disarm},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_TRANSITION_TIME});
}
TEST_F(CharacterControllerTests, ArmedSprintAndRotateState_EndForwardMoveEvent)
{
    prepareState(*this);
    tiggerAndExpect<EndForwardMoveEvent, ArmedRotateState>({sut_.animationClipsNames_.armed.rotateLeft});
}
TEST_F(CharacterControllerTests, ArmedSprintAndRotateState_RotateLeftEvent)
{
    prepareState(*this);
    expectRotationLeft();
    tiggerAndExpect<RotateLeftEvent, ArmedSprintAndRotateState>({sut_.animationClipsNames_.armed.sprint});
}
TEST_F(CharacterControllerTests, ArmedSprintAndRotateState_RotateRightEvent)
{
    prepareState(*this);
    expectRotationRight();
    tiggerAndExpect<RotateRightEvent, ArmedSprintAndRotateState>({sut_.animationClipsNames_.armed.sprint});
}
TEST_F(CharacterControllerTests, ArmedSprintAndRotateState_RotateTargetEvent)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent, ArmedSprintAndRotateState>(RotateTargetEvent{targetRotation.value_},
                                                                  {sut_.animationClipsNames_.armed.sprint});
}
TEST_F(CharacterControllerTests, ArmedSprintAndRotateState_EndRotationEvent)
{
    prepareState(*this);
    tiggerAndExpect<EndRotationEvent, ArmedSprintState>({sut_.animationClipsNames_.armed.sprint});
}
