#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    EXPECT_CALL(test.inputManagerMock_, CalcualteMouseMove()).WillRepeatedly(Return(vec2i{0, 0}));
    EXPECT_CALL(test.physicsApiMock_, GetVelocity(test.rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(test.physicsApiMock_, GetRotation(test.rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    test.expectForwardVelocity(DEFAULT_RUN_SPEED);

    test.expectState<DisarmedIdleState>();
    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.idle});
    test.tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {test.sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    test.tiggerAndExpect<DrawArrowEvent, DrawArrowState>({test.sut_.animationClipsNames_.drawArrow});
    test.tiggerAndExpect<RunForwardEvent, DrawArrowRunState>(
        {test.sut_.animationClipsNames_.drawArrow, test.sut_.animationClipsNames_.armed.run.forward});
}
}  // namespace

TEST_F(CharacterControllerTests, DrawArrowRun_RotateLeft)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));
    tiggerAndExpect<RotateLeftEvent, DrawArrowRunAndRotateState>(
        {sut_.animationClipsNames_.drawArrow, sut_.animationClipsNames_.armed.run.forward});
}
TEST_F(CharacterControllerTests, DrawArrowRun_RotateRight)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));
    tiggerAndExpect<RotateRightEvent, DrawArrowRunAndRotateState>(
        {sut_.animationClipsNames_.drawArrow, sut_.animationClipsNames_.armed.run.forward});
}
TEST_F(CharacterControllerTests, DrawArrowRun_RotateTargetEvent)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent, DrawArrowRunAndRotateState>(RotateTargetEvent{ targetRotation.value_ },
        {sut_.animationClipsNames_.drawArrow, sut_.animationClipsNames_.armed.run.forward});
}
TEST_F(CharacterControllerTests, DrawArrowRun_WeaponStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<WeaponStateEvent, RunArmedChangeState>(
        {sut_.animationClipsNames_.disarm, sut_.animationClipsNames_.disarmed.run.forward});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectState<DisarmedRunState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.run.forward});
}
TEST_F(CharacterControllerTests, DrawArrowRun_AimStartEvent)
{
    prepareState(*this);
    tiggerAndExpect<AimStartEvent, AimRunState>(
        {sut_.animationClipsNames_.aimIdle, sut_.animationClipsNames_.armed.run.forward});
}
TEST_F(CharacterControllerTests, DrawArrowRun_AimStopEvent)
{
    prepareState(*this);
    tiggerAndExpect<AimStopEvent, ArmedRunState>({sut_.animationClipsNames_.armed.run.forward});
}
TEST_F(CharacterControllerTests, DrawArrowRun_RunForwardEvent)
{
    prepareState(*this);
    tiggerAndExpect<RunForwardEvent, DrawArrowRunState>(
        {sut_.animationClipsNames_.drawArrow, sut_.animationClipsNames_.armed.run.forward});
}
TEST_F(CharacterControllerTests, DrawArrowRun_RunBackwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(-DEFAULT_BACKWARD_RUN_SPEED);
    tiggerAndExpect<RunBackwardEvent, DrawArrowRunState>(
        {sut_.animationClipsNames_.drawArrow, sut_.animationClipsNames_.armed.run.backward});
}
TEST_F(CharacterControllerTests, DrawArrowRun_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent, DeathState>({sut_.animationClipsNames_.armed.death});
}
TEST_F(CharacterControllerTests, DrawArrowRun_WalkChangeStateEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent, DrawArrowWalkState>(
        {sut_.animationClipsNames_.drawArrow, sut_.animationClipsNames_.armed.walk.forward});
}
TEST_F(CharacterControllerTests, DrawArrowRun_EndForwardMoveEvent)
{
    prepareState(*this);
    tiggerAndExpect<EndForwardMoveEvent, DrawArrowState>({sut_.animationClipsNames_.drawArrow});
}
TEST_F(CharacterControllerTests, DrawArrowRun_EndBackwardMoveEvent)
{
    prepareState(*this);
    tiggerAndExpect<EndForwardMoveEvent, DrawArrowState>({sut_.animationClipsNames_.drawArrow});
    expectForwardVelocity(-DEFAULT_BACKWARD_RUN_SPEED);
    tiggerAndExpect<RunBackwardEvent, DrawArrowRunState>(
        {sut_.animationClipsNames_.drawArrow, sut_.animationClipsNames_.armed.run.backward});
    tiggerAndExpect<EndBackwardMoveEvent, DrawArrowState>({sut_.animationClipsNames_.drawArrow});
}
TEST_F(CharacterControllerTests, DrawArrowRun_SprintStateChangeEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent, ArmedSprintState>({sut_.animationClipsNames_.armed.sprint});
}
