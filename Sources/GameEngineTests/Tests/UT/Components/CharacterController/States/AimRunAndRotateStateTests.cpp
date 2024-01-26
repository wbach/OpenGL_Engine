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
    test.Update(ADVANCED_TIME_CLIP_TIME);
    test.Update(ADVANCED_TIME_TRANSITION_TIME);
    test.expectState<AimState>();
    test.tiggerAndExpect<RunForwardEvent, AimRunState>(
        {test.sut_.animationClipsNames_.aimIdle, test.sut_.animationClipsNames_.armed.run.forward});
    test.tiggerAndExpect<RotateRightEvent, AimRunAndRotateState>(
        {test.sut_.animationClipsNames_.aimIdle, test.sut_.animationClipsNames_.armed.run.forward});
}
}  // namespace

TEST_F(CharacterControllerTests, AimRunAndRotate_RotateLeft)
{
    prepareState(*this);
    tiggerAndExpect<RotateLeftEvent, AimRunAndRotateState>(
        {sut_.animationClipsNames_.aimIdle, sut_.animationClipsNames_.armed.run.forward});
}
TEST_F(CharacterControllerTests, AimRunAndRotate_RotateRight)
{
    prepareState(*this);
    tiggerAndExpect<RotateRightEvent, AimRunAndRotateState>(
        {sut_.animationClipsNames_.aimIdle, sut_.animationClipsNames_.armed.run.forward});
}
TEST_F(CharacterControllerTests, AimRunAndRotate_WeaponStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<WeaponStateEvent, DisarmedRunAndRotateState>({sut_.animationClipsNames_.disarmed.run.forward});
}
TEST_F(CharacterControllerTests, AimRunAndRotate_AttackEvent)
{
    prepareState(*this);
    tiggerAndExpect<AttackEvent, RecoilRunAndRotateState>(
        {sut_.animationClipsNames_.recoilArrow, sut_.animationClipsNames_.armed.run.forward});
}
TEST_F(CharacterControllerTests, AimRunAndRotate_AimStopEvent)
{
    prepareState(*this);
    tiggerAndExpect<AimStopEvent, ArmedRunAndRotateState>({sut_.animationClipsNames_.armed.run.forward});
}
