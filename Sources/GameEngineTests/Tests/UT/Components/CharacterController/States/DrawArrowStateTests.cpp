#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    EXPECT_CALL(test.physicsApiMock_, SetRotation(test.rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(0));
    EXPECT_CALL(test.physicsApiMock_, GetRotation(test.rigidbodyid)).WillRepeatedly(Return(Rotation().value_));

    test.expectState<DisarmedIdleState>();
    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.idle});
    test.tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {test.sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    test.tiggerAndExpect<DrawArrowEvent, DrawArrowState>({test.sut_.animationClipsNames_.drawArrow});
}
}  // namespace

TEST_F(CharacterControllerTests, DrawArow_RotateLeft)
{
    prepareState(*this);
    tiggerAndExpect<RotateLeftEvent, DrawArrowRotateState>(
        {sut_.animationClipsNames_.drawArrow, sut_.animationClipsNames_.armed.rotateLeft});
}
TEST_F(CharacterControllerTests, DrawArow_RotateRight)
{
    prepareState(*this);
    tiggerAndExpect<RotateRightEvent, DrawArrowRotateState>(
        {sut_.animationClipsNames_.drawArrow, sut_.animationClipsNames_.armed.rotateRight});
}
TEST_F(CharacterControllerTests, DrawArow_WeaponStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<WeaponStateEvent, DisarmedIdleState>({sut_.animationClipsNames_.disarm});
}
TEST_F(CharacterControllerTests, DrawArow_AimStartEvent)
{
    prepareState(*this);
    tiggerAndExpect<AimStartEvent, AimState>({sut_.animationClipsNames_.aimIdle});
}
TEST_F(CharacterControllerTests, DrawArow_AimStopEvent)
{
    prepareState(*this);
    tiggerAndExpect<AimStopEvent, ArmedIdleState>({sut_.animationClipsNames_.armed.idle});
}
