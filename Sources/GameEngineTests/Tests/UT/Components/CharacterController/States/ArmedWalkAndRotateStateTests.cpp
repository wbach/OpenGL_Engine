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

    test.expectForwardVelocity(DEFAULT_RUN_SPEED);
    test.tiggerAndExpect<MoveForwardEvent, ArmedWalkState>({test.sut_.animationClipsNames_.armed.run.forward});
    test.expectForwardVelocity(DEFAULT_WALK_SPEED);
    test.tiggerAndExpect<WalkChangeStateEvent, ArmedWalkState>({test.sut_.animationClipsNames_.armed.walk.forward});
    test.expectRotationRight();
    test.tiggerAndExpect<RotateRightEvent, ArmedWalkAndRotateState>(
        {test.sut_.animationClipsNames_.armed.walk.forward});
}
}  // namespace

TEST_F(CharacterControllerTests, ArmedWalkAndRotate_DrawArrowEvent)
{
    prepareState(*this);
    tiggerAndExpect<DrawArrowEvent, DrawArrowWalkAndRotateState>(
        {sut_.animationClipsNames_.drawArrow, sut_.animationClipsNames_.armed.walk.forward});
}

TEST_F(CharacterControllerTests, ArmedWalkAndRotate_RotateLeftEvent)
{
    prepareState(*this);
    expectRotationLeft();
    tiggerAndExpect<RotateLeftEvent, ArmedWalkAndRotateState>({sut_.animationClipsNames_.armed.walk.forward});
}

TEST_F(CharacterControllerTests, ArmedWalkAndRotate_RotateRightEvent)
{
    prepareState(*this);
    expectRotationLeft();
    tiggerAndExpect<RotateLeftEvent, ArmedWalkAndRotateState>({sut_.animationClipsNames_.armed.walk.forward});
    expectRotationRight();
    tiggerAndExpect<RotateRightEvent, ArmedWalkAndRotateState>({sut_.animationClipsNames_.armed.walk.forward});
}

TEST_F(CharacterControllerTests, ArmedWalkAndRotate_RotateTargetEvent)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent, ArmedWalkAndRotateState>(RotateTargetEvent{targetRotation.value_},
                                                                {sut_.animationClipsNames_.armed.walk.forward});
}

TEST_F(CharacterControllerTests, ArmedWalkAndRotate_MoveForwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<MoveForwardEvent, ArmedWalkAndRotateState>({sut_.animationClipsNames_.armed.walk.forward});
}

TEST_F(CharacterControllerTests, ArmedWalkAndRotate_MoveBackwardEvent)
{
    prepareState(*this);
    tiggerAndExpect<EndForwardMoveEvent, ArmedRotateState>({sut_.animationClipsNames_.armed.rotateRight});
    expectForwardVelocity(-DEFAULT_WALK_SPEED);
    tiggerAndExpect<MoveBackwardEvent, ArmedWalkAndRotateState>({sut_.animationClipsNames_.armed.walk.forward});
    expectRootboneRotation(VECTOR_BACKWARD);
}

TEST_F(CharacterControllerTests, ArmedWalkAndRotate_MoveLeftEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_LEFT, vec3(DEFAULT_WALK_LEFT_RIGHT_SPEED, 0.0, 0));
    tiggerAndExpect<MoveLeftEvent, ArmedWalkAndRotateState>({sut_.animationClipsNames_.armed.walk.forward});
    expectRootboneRotation(VECTOR_LEFT);
}

TEST_F(CharacterControllerTests, ArmedWalkAndRotate_MoveRightEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_RIGHT, vec3(DEFAULT_WALK_LEFT_RIGHT_SPEED, 0.0, 0));
    tiggerAndExpect<MoveRightEvent, ArmedWalkAndRotateState>({sut_.animationClipsNames_.armed.walk.forward});
    expectRootboneRotation(VECTOR_RIGHT);
}

TEST_F(CharacterControllerTests, ArmedWalkAndRotate_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent, DeathState>({sut_.animationClipsNames_.armed.death});
}

TEST_F(CharacterControllerTests, ArmedWalkAndRotate_WalkChangeStateEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<WalkChangeStateEvent, ArmedRunAndRotateState>({sut_.animationClipsNames_.armed.run.forward});
}

TEST_F(CharacterControllerTests, ArmedWalkAndRotate_WeaponStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<WeaponStateEvent, WalkAndRotateArmedChangeState>(
        {sut_.animationClipsNames_.disarmed.walk.forward, sut_.animationClipsNames_.disarm});
}

TEST_F(CharacterControllerTests, ArmedWalkAndRotate_EndForwardMoveEvent)
{
    prepareState(*this);
    tiggerAndExpect<EndForwardMoveEvent, ArmedRotateState>({sut_.animationClipsNames_.armed.rotateRight});
}

TEST_F(CharacterControllerTests, ArmedWalkAndRotate_EndBackwardMoveEvent)
{
    prepareState(*this);
    expectNoMove();
    tiggerAndExpect<MoveBackwardEvent, ArmedWalkAndRotateState>({sut_.animationClipsNames_.armed.rotateRight});
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<EndBackwardMoveEvent, ArmedWalkAndRotateState>({sut_.animationClipsNames_.armed.walk.forward});
}

TEST_F(CharacterControllerTests, ArmedWalkAndRotate_EndRotationEvent)
{
    prepareState(*this);
    tiggerAndExpect<EndRotationEvent, ArmedWalkState>({sut_.animationClipsNames_.armed.walk.forward});
}

TEST_F(CharacterControllerTests, ArmedWalkAndRotate_SprintStateChangeEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent, ArmedSprintAndRotateState>({sut_.animationClipsNames_.armed.sprint});
}

TEST_F(CharacterControllerTests, ArmedWalkAndRotate_SprintStartEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent, ArmedSprintAndRotateState>({sut_.animationClipsNames_.armed.sprint});
}
