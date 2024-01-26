#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    EXPECT_CALL(test.physicsApiMock_, SetRotation(test.rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));
    EXPECT_CALL(test.physicsApiMock_, GetRotation(test.rigidbodyid)).WillRepeatedly(Return(Rotation().value_));

    test.expectState<DisarmedIdleState>();
    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.idle});
    test.tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {test.sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    test.tiggerAndExpect<DrawArrowEvent, DrawArrowState>({test.sut_.animationClipsNames_.drawArrow});
    test.tiggerAndExpect<WalkForwardEvent, DrawArrowWalkState>(
        {test.sut_.animationClipsNames_.drawArrow, test.sut_.animationClipsNames_.armed.walk.forward});
    test.tiggerAndExpect<RotateRightEvent, DrawArrowWalkAndRotateState>(
        {test.sut_.animationClipsNames_.drawArrow, test.sut_.animationClipsNames_.armed.walk.forward});
}
}  // namespace

TEST_F(CharacterControllerTests, DrawArrowWalkAndRotate_RotateLeft)
{
    prepareState(*this);
    tiggerAndExpect<RotateLeftEvent, DrawArrowWalkAndRotateState>(
        {sut_.animationClipsNames_.drawArrow, sut_.animationClipsNames_.armed.walk.forward});
}
TEST_F(CharacterControllerTests, DrawArrowWalkAndRotate_RotateRight)
{
    prepareState(*this);
    tiggerAndExpect<RotateRightEvent, DrawArrowWalkAndRotateState>(
        {sut_.animationClipsNames_.drawArrow, sut_.animationClipsNames_.armed.walk.forward});
}
TEST_F(CharacterControllerTests, DrawArrowWalkAndRotate_WeaponStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<WeaponStateEvent, DisarmedWalkAndRotateState>({sut_.animationClipsNames_.disarmed.walk.forward});
}
TEST_F(CharacterControllerTests, DrawArrowWalkAndRotate_AimStartEvent)
{
    prepareState(*this);
    tiggerAndExpect<AimStartEvent, AimWalkAndRotateState>(
        {sut_.animationClipsNames_.aimIdle, sut_.animationClipsNames_.armed.walk.forward});
}
TEST_F(CharacterControllerTests, DrawArrowWalkAndRotate_AimStopEvent)
{
    prepareState(*this);
    tiggerAndExpect<AimStopEvent, ArmedWalkAndRotateState>({sut_.animationClipsNames_.armed.walk.forward});
}
