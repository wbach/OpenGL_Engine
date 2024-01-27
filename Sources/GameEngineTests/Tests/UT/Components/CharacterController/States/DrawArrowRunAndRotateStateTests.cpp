#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    EXPECT_CALL(test.physicsApiMock_, SetRotation(test.rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));
    EXPECT_CALL(test.physicsApiMock_, GetRotation(test.rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(test.physicsApiMock_, GetVelocity(test.rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    test.expectForwardVelocity(DEFAULT_RUN_SPEED);

    test.expectState<DisarmedIdleState>();
    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.idle});
    test.tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {test.sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    test.tiggerAndExpect<DrawArrowEvent, DrawArrowState>({test.sut_.animationClipsNames_.drawArrow});
    test.tiggerAndExpect<RunForwardEvent, DrawArrowRunState>(
        {test.sut_.animationClipsNames_.drawArrow, test.sut_.animationClipsNames_.armed.run.forward});
    test.tiggerAndExpect<RotateRightEvent, DrawArrowRunAndRotateState>(
        {test.sut_.animationClipsNames_.drawArrow, test.sut_.animationClipsNames_.armed.run.forward});
}
}  // namespace

TEST_F(CharacterControllerTests, DrawArrowRunAndRotate_RotateLeft)
{
    prepareState(*this);
    tiggerAndExpect<RotateLeftEvent, DrawArrowRunAndRotateState>(
        {sut_.animationClipsNames_.drawArrow, sut_.animationClipsNames_.armed.run.forward});
}
TEST_F(CharacterControllerTests, DrawArrowRunAndRotate_RotateRight)
{
    prepareState(*this);
    tiggerAndExpect<RotateRightEvent, DrawArrowRunAndRotateState>(
        {sut_.animationClipsNames_.drawArrow, sut_.animationClipsNames_.armed.run.forward});
}
TEST_F(CharacterControllerTests, DrawArrowRunAndRotate_WeaponStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<WeaponStateEvent, DisarmedRunAndRotateState>(
        {sut_.animationClipsNames_.disarm, sut_.animationClipsNames_.disarmed.run.forward});
}
TEST_F(CharacterControllerTests, DrawArrowRunAndRotate_AimStartEvent)
{
    prepareState(*this);
    tiggerAndExpect<AimStartEvent, AimRunAndRotateState>(
        {sut_.animationClipsNames_.aimIdle, sut_.animationClipsNames_.armed.run.forward});
}
TEST_F(CharacterControllerTests, DrawArrowRunAndRotate_AimStopEvent)
{
    prepareState(*this);
    tiggerAndExpect<AimStopEvent, ArmedRunAndRotateState>({sut_.animationClipsNames_.armed.run.forward});
}
