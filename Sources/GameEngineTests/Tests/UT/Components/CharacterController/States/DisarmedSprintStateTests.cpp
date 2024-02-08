#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    EXPECT_CALL(test.physicsApiMock_, GetVelocity(test.rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(test.physicsApiMock_, GetRotation(test.rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    test.expectForwardVelocity(DEFAULT_SPRINT_SPEED);

    test.expectState<DisarmedIdleState>();
    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.idle});
    test.tiggerAndExpect<SprintStartEvent, DisarmedSprintState>({test.sut_.animationClipsNames_.disarmed.sprint});
}
}  // namespace

TEST_F(CharacterControllerTests, DisarmedSprintState_DrawArrowEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<DrawArrowEvent, RunArmedChangeState>(
        {sut_.animationClipsNames_.disarmed.run.forward, sut_.animationClipsNames_.equip},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME / 2.f});
    Update(ADVANCED_TIME_CLIP_TIME / 2.f);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectState<DrawArrowRunState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.run.forward, sut_.animationClipsNames_.drawArrow});
}
TEST_F(CharacterControllerTests, DisarmedSprintState_RunForwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<RunForwardEvent, DisarmedRunState>({sut_.animationClipsNames_.disarmed.run.forward});
}
TEST_F(CharacterControllerTests, DisarmedSprintState_RunBackwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(-DEFAULT_BACKWARD_RUN_SPEED);
    tiggerAndExpect<RunBackwardEvent, DisarmedRunState>({sut_.animationClipsNames_.disarmed.run.backward});
}
TEST_F(CharacterControllerTests, DisarmedSprintState_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent, DeathState>({sut_.animationClipsNames_.disarmed.death});
}
TEST_F(CharacterControllerTests, DisarmedSprintState_WalkChangeStateEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent, DisarmedWalkState>({sut_.animationClipsNames_.disarmed.walk.forward});
}
TEST_F(CharacterControllerTests, DisarmedSprintState_SprintStateChangeEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<SprintStateChangeEvent, DisarmedRunState>({sut_.animationClipsNames_.disarmed.run.forward});
}
TEST_F(CharacterControllerTests, DisarmedSprintState_WeaponStateEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<WeaponStateEvent, RunArmedChangeState>(
        {sut_.animationClipsNames_.disarmed.run.forward, sut_.animationClipsNames_.equip},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME / 2.f});
}
TEST_F(CharacterControllerTests, DisarmedSprintState_EndForwardMoveEvent)
{
    prepareState(*this);
    tiggerAndExpect<EndForwardMoveEvent, DisarmedIdleState>({sut_.animationClipsNames_.disarmed.idle});
}
TEST_F(CharacterControllerTests, DisarmedSprintState_RotateLeftEvent)
{
    prepareState(*this);
    expectRotationLeft();
    tiggerAndExpect<RotateLeftEvent, DisarmedSprintAndRotateState>({sut_.animationClipsNames_.disarmed.sprint});
}
TEST_F(CharacterControllerTests, DisarmedSprintState_RotateRightEvent)
{
    prepareState(*this);
    expectRotationRight();
    tiggerAndExpect<RotateRightEvent, DisarmedSprintAndRotateState>({sut_.animationClipsNames_.disarmed.sprint});
}
TEST_F(CharacterControllerTests, DisarmedSprintState_RotateTargetEvent)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent, DisarmedSprintAndRotateState>(RotateTargetEvent{targetRotation.value_},
                                                                  {sut_.animationClipsNames_.disarmed.sprint});
}
TEST_F(CharacterControllerTests, DisarmedSprintState_AimStopEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<DrawArrowEvent, RunArmedChangeState>(
        {sut_.animationClipsNames_.disarmed.run.forward, sut_.animationClipsNames_.equip},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME / 2.f});
    tiggerAndExpect<AimStopEvent, ArmedRunState>({sut_.animationClipsNames_.armed.run.forward},
                                                 {ADVANCED_TIME_CLIP_TIME / 2.f, ADVANCED_TIME_TRANSITION_TIME});
}
