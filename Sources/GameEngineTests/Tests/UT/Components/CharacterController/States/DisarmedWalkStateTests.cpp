#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    EXPECT_CALL(test.physicsApiMock_, GetVelocity(test.rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(test.physicsApiMock_, GetRotation(test.rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    test.expectForwardVelocity(DEFAULT_WALK_SPEED);

    test.expectState<DisarmedIdleState>();
    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.idle});
    test.tiggerAndExpect<WalkForwardEvent, DisarmedWalkState>({test.sut_.animationClipsNames_.disarmed.walk.forward});
}
}  // namespace

TEST_F(CharacterControllerTests, DisarmedWalkState_DrawArrowEvent)
{
    prepareState(*this);
    tiggerAndExpect<DrawArrowEvent, WalkArmedChangeState>(
        {sut_.animationClipsNames_.armed.walk.forward, sut_.animationClipsNames_.equip},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME / 2.f});
    Update(ADVANCED_TIME_CLIP_TIME / 2.f);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectState<DrawArrowWalkState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.walk.forward, sut_.animationClipsNames_.drawArrow});
}

TEST_F(CharacterControllerTests, DisarmedWalkState_AimStopDuringDrawArrowEvent)
{
    prepareState(*this);
    tiggerAndExpect<DrawArrowEvent, WalkArmedChangeState>(
        {sut_.animationClipsNames_.armed.walk.forward, sut_.animationClipsNames_.equip},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME / 2.f});
    tiggerAndExpect<AimStopEvent, ArmedWalkState>({sut_.animationClipsNames_.armed.walk.forward},
                                                 {ADVANCED_TIME_CLIP_TIME / 2.f, ADVANCED_TIME_TRANSITION_TIME});

    // Extra check
    Update(ADVANCED_TIME_CLIP_TIME);
    expectState<ArmedWalkState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.walk.forward});
}

TEST_F(CharacterControllerTests, DisarmedWalkState_RunForwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<RunForwardEvent, DisarmedRunState>({sut_.animationClipsNames_.disarmed.run.forward});
}

TEST_F(CharacterControllerTests, DisarmedWalkState_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent, DeathState>({sut_.animationClipsNames_.disarmed.death});
}

TEST_F(CharacterControllerTests, DisarmedWalkState_WalkChangeStateEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<WalkChangeStateEvent, DisarmedRunState>({sut_.animationClipsNames_.disarmed.run.forward});
}

TEST_F(CharacterControllerTests, DisarmedWalkState_WeaponStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<WeaponStateEvent, WalkArmedChangeState>(
        {sut_.animationClipsNames_.armed.walk.forward, sut_.animationClipsNames_.equip});
}

TEST_F(CharacterControllerTests, DisarmedWalkState_EndForwardMoveEvent)
{
    prepareState(*this);
    tiggerAndExpect<EndForwardMoveEvent, DisarmedIdleState>({sut_.animationClipsNames_.disarmed.idle});
}

TEST_F(CharacterControllerTests, DisarmedWalkState_EndBackwardMoveEvent)
{
    prepareState(*this);
    tiggerAndExpect<EndForwardMoveEvent, DisarmedIdleState>({ sut_.animationClipsNames_.disarmed.idle });
    expectForwardVelocity(-DEFAULT_BACKWARD_WALK_SPEED);
    tiggerAndExpect<WalkBackwardEvent, DisarmedWalkState>({ sut_.animationClipsNames_.disarmed.walk.backward });
    tiggerAndExpect<EndBackwardMoveEvent, DisarmedIdleState>({ sut_.animationClipsNames_.disarmed.idle });
}

TEST_F(CharacterControllerTests, DisarmedWalkState_RunBackwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(-DEFAULT_BACKWARD_RUN_SPEED);
    tiggerAndExpect<RunBackwardEvent, DisarmedRunState>({sut_.animationClipsNames_.disarmed.run.backward});
}

TEST_F(CharacterControllerTests, DisarmedWalkState_RotateLeftEvent)
{
    prepareState(*this);
    expectRotationLeft();
    tiggerAndExpect<RotateLeftEvent, DisarmedWalkAndRotateState>({sut_.animationClipsNames_.disarmed.walk.forward});
}

TEST_F(CharacterControllerTests, DisarmedWalkState_RotateRightEvent)
{
    prepareState(*this);
    expectRotationRight();
    tiggerAndExpect<RotateRightEvent, DisarmedWalkAndRotateState>({sut_.animationClipsNames_.disarmed.walk.forward});
}

TEST_F(CharacterControllerTests, DisarmedWalkState_RotateTargetEvent)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent, DisarmedWalkAndRotateState>(RotateTargetEvent{targetRotation.value_},
                                                                  {sut_.animationClipsNames_.disarmed.walk.forward});
}

TEST_F(CharacterControllerTests, DisarmedWalkState_SprintStateChangeEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent, DisarmedSprintState>({sut_.animationClipsNames_.disarmed.sprint});
}
