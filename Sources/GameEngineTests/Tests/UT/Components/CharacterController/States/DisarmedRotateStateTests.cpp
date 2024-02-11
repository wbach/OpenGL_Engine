#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    EXPECT_CALL(test.physicsApiMock_, GetVelocity(test.rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(test.physicsApiMock_, GetRotation(test.rigidbodyid)).WillRepeatedly(Return(Rotation().value_));

    test.expectState<DisarmedIdleState>();
    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.idle});
    test.expectRotationLeft();
    test.tiggerAndExpect<RotateLeftEvent, DisarmedRotateState>({test.sut_.animationClipsNames_.disarmed.rotateLeft});
}
}  // namespace

TEST_F(CharacterControllerTests, DisarmedRotateState_DrawArrowEvent)
{
    prepareState(*this);
    expectRotationLeft(ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<DrawArrowEvent, RotateArmedChangeState>(
        {sut_.animationClipsNames_.armed.rotateLeft, sut_.animationClipsNames_.equip},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_TRANSITION_TIME});
    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectRotationLeft(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectState<DrawArrowRotateState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.rotateLeft, sut_.animationClipsNames_.drawArrow});
}

TEST_F(CharacterControllerTests, DisarmedRotateState_AimStopDuringDrawArrowEvent)
{
    prepareState(*this);
    expectRotationLeft(ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<DrawArrowEvent, RotateArmedChangeState>(
        {sut_.animationClipsNames_.armed.rotateLeft, sut_.animationClipsNames_.equip},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_TRANSITION_TIME});

    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    tiggerAndExpect<AimStopEvent, ArmedRotateState>({sut_.animationClipsNames_.armed.rotateLeft},
                                                    {ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_CLIP_TIME});

    // Extra check
    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectState<ArmedRotateState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.rotateLeft});
}

TEST_F(CharacterControllerTests, DisarmedRotateState_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent, DeathState>({sut_.animationClipsNames_.disarmed.death});
}

TEST_F(CharacterControllerTests, DisarmedRotateState_WeaponStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<WeaponStateEvent, RotateArmedChangeState>(
        {sut_.animationClipsNames_.armed.rotateLeft, sut_.animationClipsNames_.equip});
    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectRotationLeft();
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectState<ArmedRotateState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.rotateLeft});
}

TEST_F(CharacterControllerTests, DisarmedRotateState_RunForwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<RunForwardEvent, DisarmedRunAndRotateState>({sut_.animationClipsNames_.disarmed.run.forward});
}

TEST_F(CharacterControllerTests, DisarmedRotateState_RunBackwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(-DEFAULT_BACKWARD_RUN_SPEED);
    tiggerAndExpect<RunBackwardEvent, DisarmedRunAndRotateState>({sut_.animationClipsNames_.disarmed.run.backward});
}

TEST_F(CharacterControllerTests, DisarmedRotateState_WalkForwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkForwardEvent, DisarmedWalkAndRotateState>({sut_.animationClipsNames_.disarmed.walk.forward});
}

TEST_F(CharacterControllerTests, DisarmedRotateState_WalkBackwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(-DEFAULT_BACKWARD_WALK_SPEED);
    tiggerAndExpect<WalkBackwardEvent, DisarmedWalkAndRotateState>({sut_.animationClipsNames_.disarmed.walk.backward});
}

TEST_F(CharacterControllerTests, DisarmedRotateState_RotateLeftEvent)
{
    prepareState(*this);
    tiggerAndExpect<RotateLeftEvent, DisarmedRotateState>({sut_.animationClipsNames_.disarmed.rotateLeft});
}

TEST_F(CharacterControllerTests, DisarmedRotateState_RotateRightEvent)
{
    prepareState(*this);
    expectRotationRight();
    tiggerAndExpect<RotateRightEvent, DisarmedRotateState>({sut_.animationClipsNames_.disarmed.rotateRight});
}

TEST_F(CharacterControllerTests, DisarmedRotateState_RotateTargetEvent)
{
    prepareState(*this);

    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent, DisarmedRotateState>(RotateTargetEvent{targetRotation.value_},
                                                            {sut_.animationClipsNames_.disarmed.rotateLeft});
}

TEST_F(CharacterControllerTests, DisarmedRotateState_EndRotationEvent)
{
    prepareState(*this);
    tiggerAndExpect<EndRotationEvent, DisarmedIdleState>({sut_.animationClipsNames_.disarmed.idle});
}

TEST_F(CharacterControllerTests, DisarmedRotateState_SprintStartEvent)
{
    prepareState(*this);
    tiggerAndExpect<SprintStartEvent, DisarmedSprintAndRotateState>({sut_.animationClipsNames_.disarmed.sprint});
}
