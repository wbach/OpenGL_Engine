#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    EXPECT_CALL(test.physicsApiMock_, GetVelocity(test.rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(test.physicsApiMock_, GetRotation(test.rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    test.expectForwardVelocity(DEFAULT_RUN_SPEED);
    test.expectRotationLeft();

    test.expectState<DisarmedIdleState>();
    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.idle});
    test.tiggerAndExpect<RunForwardEvent, DisarmedRunState>({test.sut_.animationClipsNames_.disarmed.run.forward});
    test.tiggerAndExpect<RotateLeftEvent, DisarmedRunAndRotateState>(
        {test.sut_.animationClipsNames_.disarmed.run.forward});
}
}  // namespace

TEST_F(CharacterControllerTests, DisarmedRunAndRotateState_DrawArrowEvent)
{
    prepareState(*this);
    tiggerAndExpect<DrawArrowEvent, RunAndRotateArmedChangeState>(
        {sut_.animationClipsNames_.armed.run.forward, sut_.animationClipsNames_.equip},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_TRANSITION_TIME });
    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectRotationLeft();
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectState<DrawArrowWalkAndRotateState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.walk.forward, sut_.animationClipsNames_.drawArrow});
}

TEST_F(CharacterControllerTests, DisarmedRunAndRotateState_AimStopDuringDrawArrowEvent)
{
    prepareState(*this);
    tiggerAndExpect<DrawArrowEvent, RunAndRotateArmedChangeState>(
        {sut_.animationClipsNames_.armed.run.forward, sut_.animationClipsNames_.equip},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_TRANSITION_TIME});
    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    tiggerAndExpect<AimStopEvent, ArmedRunAndRotateState>({sut_.animationClipsNames_.armed.run.forward},
                                                 {ADVANCED_TIME_CLIP_TIME , ADVANCED_TIME_CLIP_TIME });

    // Extra check
    Update(ADVANCED_TIME_CLIP_TIME);
    expectState<ArmedRunAndRotateState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.run.forward});
}

TEST_F(CharacterControllerTests, DisarmedRunAndRotateState_RunForwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(-DEFAULT_BACKWARD_RUN_SPEED);
    tiggerAndExpect<RunBackwardEvent, DisarmedRunAndRotateState>({sut_.animationClipsNames_.disarmed.run.backward});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<RunForwardEvent, DisarmedRunAndRotateState>({sut_.animationClipsNames_.disarmed.run.forward});
}

TEST_F(CharacterControllerTests, DisarmedRunAndRotateState_RunBackwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(-DEFAULT_BACKWARD_RUN_SPEED);
    tiggerAndExpect<RunBackwardEvent, DisarmedRunAndRotateState>({sut_.animationClipsNames_.disarmed.run.backward});
}

TEST_F(CharacterControllerTests, DisarmedRunAndRotateState_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent, DeathState>({sut_.animationClipsNames_.disarmed.death});
}

TEST_F(CharacterControllerTests, DisarmedRunAndRotateState_WalkChangeStateEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent, DisarmedWalkAndRotateState>({sut_.animationClipsNames_.disarmed.walk.forward});
}

TEST_F(CharacterControllerTests, DisarmedRunAndRotateState_WeaponStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<WeaponStateEvent, RunAndRotateArmedChangeState>(
        {sut_.animationClipsNames_.armed.run.forward, sut_.animationClipsNames_.equip});
}

TEST_F(CharacterControllerTests, DisarmedRunAndRotateState_EndForwardMoveEvent)
{
    prepareState(*this);
    tiggerAndExpect<EndForwardMoveEvent, DisarmedRotateState>({sut_.animationClipsNames_.disarmed.rotateLeft});
}

TEST_F(CharacterControllerTests, DisarmedRunAndRotateState_EndBackwardMoveEvent)
{
    prepareState(*this);
    expectForwardVelocity(-DEFAULT_BACKWARD_RUN_SPEED);
    tiggerAndExpect<RunBackwardEvent, DisarmedRunAndRotateState>({sut_.animationClipsNames_.disarmed.run.backward});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<EndBackwardMoveEvent, DisarmedRunAndRotateState>({sut_.animationClipsNames_.disarmed.run.forward});
    tiggerAndExpect<EndForwardMoveEvent, DisarmedRotateState>({sut_.animationClipsNames_.disarmed.rotateLeft});
}

TEST_F(CharacterControllerTests, DisarmedRunAndRotateState_RotateLeftEvent)
{
    prepareState(*this);
    expectRotationLeft();
    tiggerAndExpect<RotateLeftEvent, DisarmedRunAndRotateState>({sut_.animationClipsNames_.disarmed.run.forward});
}

TEST_F(CharacterControllerTests, DisarmedRunAndRotateState_RotateRightEvent)
{
    prepareState(*this);
    expectRotationRight();
    tiggerAndExpect<RotateRightEvent, DisarmedRunAndRotateState>({sut_.animationClipsNames_.disarmed.run.forward});
}

TEST_F(CharacterControllerTests, DisarmedRunAndRotateState_RotateTargetEvent)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent, DisarmedRunAndRotateState>(RotateTargetEvent{targetRotation.value_},
                                                                  {sut_.animationClipsNames_.disarmed.run.forward});
}

TEST_F(CharacterControllerTests, DisarmedRunAndRotateState_SprintStateChangeEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent, DisarmedSprintAndRotateState>({sut_.animationClipsNames_.disarmed.sprint});
}

TEST_F(CharacterControllerTests, DisarmedRunAndRotateState_EndRotationEvent)
{
    prepareState(*this);
    tiggerAndExpect<EndRotationEvent, DisarmedRunState>({ sut_.animationClipsNames_.disarmed.run.forward });
}
