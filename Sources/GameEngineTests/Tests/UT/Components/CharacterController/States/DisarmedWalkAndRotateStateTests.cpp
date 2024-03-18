#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    EXPECT_CALL(test.physicsApiMock_, GetVelocity(test.rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(test.physicsApiMock_, GetRotation(test.rigidbodyid)).WillRepeatedly(Return(Rotation().value_));

    test.expectRotationLeft();

    test.expectState<DisarmedIdleState>();
    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.idle});

    test.expectForwardVelocity(DEFAULT_RUN_SPEED);
    test.tiggerAndExpect<MoveForwardEvent, DisarmedWalkState>({test.sut_.animationClipsNames_.disarmed.run.forward});
    test.expectForwardVelocity(DEFAULT_WALK_SPEED);
    test.tiggerAndExpect<WalkChangeStateEvent, DisarmedWalkState>(
        {test.sut_.animationClipsNames_.disarmed.walk.forward});
    test.tiggerAndExpect<RotateLeftEvent, DisarmedWalkAndRotateState>(
        {test.sut_.animationClipsNames_.disarmed.walk.forward});
}
}  // namespace

TEST_F(CharacterControllerTests, DisarmedWalkAndRotateState_DrawArrowEvent)
{
    prepareState(*this);
    tiggerAndExpect<DrawArrowEvent, WalkAndRotateArmedChangeState>(
        {sut_.animationClipsNames_.armed.walk.forward, sut_.animationClipsNames_.equip},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_TRANSITION_TIME});
    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectRotationLeft();
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectState<DrawArrowWalkAndRotateState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.walk.forward, sut_.animationClipsNames_.drawArrow});
}

TEST_F(CharacterControllerTests, DisarmedWalkAndRotateState_AimStopDuringDrawArrowEvent)
{
    prepareState(*this);
    tiggerAndExpect<DrawArrowEvent, WalkAndRotateArmedChangeState>(
        {sut_.animationClipsNames_.armed.walk.forward, sut_.animationClipsNames_.equip},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_TRANSITION_TIME});
    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    tiggerAndExpect<AimStopEvent, ArmedWalkAndRotateState>({sut_.animationClipsNames_.armed.walk.forward},
                                                           {ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_CLIP_TIME});

    // Extra check
    Update(ADVANCED_TIME_CLIP_TIME);
    expectState<ArmedWalkAndRotateState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.walk.forward});
}

TEST_F(CharacterControllerTests, DisarmedWalkAndRotateState_MoveForwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<MoveForwardEvent, DisarmedRunAndRotateState>({sut_.animationClipsNames_.disarmed.run.forward});
}

TEST_F(CharacterControllerTests, DisarmedWalkAndRotateState_RunBackwardEvent)
{
    prepareState(*this);
    expectNoMove();
    tiggerAndExpect<MoveBackwardEvent, DisarmedRunAndRotateState>({sut_.animationClipsNames_.disarmed.rotateLeft});
    expectForwardVelocity(-DEFAULT_WALK_SPEED);
    tiggerAndExpect<EndForwardMoveEvent, DisarmedRunAndRotateState>({sut_.animationClipsNames_.disarmed.run.backward});
    expectRootboneRotation(VECTOR_BACKWARD);
}

TEST_F(CharacterControllerTests, DisarmedWalkAndRotateState_MoveLeftEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_LEFT, vec3(DEFAULT_WALK_SPEED, 0.0, 0));
    tiggerAndExpect<MoveLeftEvent, DisarmedRunAndRotateState>({sut_.animationClipsNames_.disarmed.run.forward});
    expectRootboneRotation(VECTOR_LEFT);
}

TEST_F(CharacterControllerTests, DisarmedWalkAndRotateState_MoveRightEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_RIGHT, vec3(DEFAULT_WALK_SPEED, 0.0, 0));
    tiggerAndExpect<MoveRightEvent, DisarmedRunAndRotateState>({sut_.animationClipsNames_.disarmed.run.forward});
    expectRootboneRotation(VECTOR_RIGHT);
}

TEST_F(CharacterControllerTests, DisarmedWalkAndRotateState_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent, DeathState>({sut_.animationClipsNames_.disarmed.death});
}

TEST_F(CharacterControllerTests, DisarmedWalkAndRotateState_WalkChangeStateEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<WalkChangeStateEvent, DisarmedRunAndRotateState>({sut_.animationClipsNames_.disarmed.run.forward});
}

TEST_F(CharacterControllerTests, DisarmedWalkAndRotateState_WeaponStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<WeaponStateEvent, WalkAndRotateArmedChangeState>(
        {sut_.animationClipsNames_.armed.walk.forward, sut_.animationClipsNames_.equip});
}

TEST_F(CharacterControllerTests, DisarmedWalkAndRotateState_EndForwardMoveEvent)
{
    prepareState(*this);
    tiggerAndExpect<EndForwardMoveEvent, DisarmedRotateState>({sut_.animationClipsNames_.disarmed.rotateLeft});
}

TEST_F(CharacterControllerTests, DisarmedWalkAndRotateState_EndBackwardMoveEvent)
{
    prepareState(*this);
    tiggerAndExpect<EndForwardMoveEvent, DisarmedRotateState>({sut_.animationClipsNames_.disarmed.rotateLeft});

    expectForwardVelocity(-DEFAULT_WALK_SPEED);
    tiggerAndExpect<MoveBackwardEvent, DisarmedWalkAndRotateState>({sut_.animationClipsNames_.disarmed.walk.backward});
    tiggerAndExpect<EndBackwardMoveEvent, DisarmedRotateState>({sut_.animationClipsNames_.disarmed.rotateLeft});
}

TEST_F(CharacterControllerTests, DisarmedWalkAndRotateState_RotateLeftEvent)
{
    prepareState(*this);
    expectRotationLeft();
    tiggerAndExpect<RotateLeftEvent, DisarmedWalkAndRotateState>({sut_.animationClipsNames_.disarmed.walk.forward});
}

TEST_F(CharacterControllerTests, DisarmedWalkAndRotateState_RotateRightEvent)
{
    prepareState(*this);
    expectRotationRight();
    tiggerAndExpect<RotateRightEvent, DisarmedWalkAndRotateState>({sut_.animationClipsNames_.disarmed.walk.forward});
}

TEST_F(CharacterControllerTests, DisarmedWalkAndRotateState_RotateTargetEvent)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent, DisarmedWalkAndRotateState>(RotateTargetEvent{targetRotation.value_},
                                                                   {sut_.animationClipsNames_.disarmed.walk.forward});
}

TEST_F(CharacterControllerTests, DisarmedWalkAndRotateState_SprintStateChangeEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent, DisarmedSprintAndRotateState>({sut_.animationClipsNames_.disarmed.sprint});
}

TEST_F(CharacterControllerTests, DisarmedWalkAndRotateState_EndRotationEvent)
{
    prepareState(*this);
    tiggerAndExpect<EndRotationEvent, DisarmedWalkState>({sut_.animationClipsNames_.disarmed.walk.forward});
}
