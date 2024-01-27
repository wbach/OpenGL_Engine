#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    EXPECT_CALL(test.physicsApiMock_, GetRotation(test.rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(test.physicsApiMock_, GetVelocity(test.rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    test.expectForwardVelocity(DEFAULT_RUN_SPEED);

    test.expectState<DisarmedIdleState>();
    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.idle});
    test.tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {test.sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    test.tiggerAndExpect<DrawArrowEvent, DrawArrowState>({test.sut_.animationClipsNames_.drawArrow});
    test.Update(ADVANCED_TIME_CLIP_TIME);
    test.Update(ADVANCED_TIME_TRANSITION_TIME);
    test.expectState<AimState>();
    test.tiggerAndExpect<AttackEvent, RecoilState>({test.sut_.animationClipsNames_.recoilArrow});
    test.tiggerAndExpect<RunForwardEvent, RecoilRunState>(
        {test.sut_.animationClipsNames_.recoilArrow, test.sut_.animationClipsNames_.armed.run.forward});
}
}  // namespace

TEST_F(CharacterControllerTests, RecoilRun_RotateLeft)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));
    tiggerAndExpect<RotateLeftEvent, RecoilRunAndRotateState>(
        {sut_.animationClipsNames_.recoilArrow, sut_.animationClipsNames_.armed.run.forward});
}
TEST_F(CharacterControllerTests, RecoilRun_RotateRight)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));
    tiggerAndExpect<RotateRightEvent, RecoilRunAndRotateState>(
        {sut_.animationClipsNames_.recoilArrow, sut_.animationClipsNames_.armed.run.forward});
}
TEST_F(CharacterControllerTests, RecoilRun_WeaponStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<WeaponStateEvent, DisarmedRunState>({sut_.animationClipsNames_.disarmed.run.forward, sut_.animationClipsNames_.disarm});
}
TEST_F(CharacterControllerTests, RecoilRun_DrawArrowEvent)
{
    prepareState(*this);
    tiggerAndExpect<DrawArrowEvent, DrawArrowRunState>(
        {sut_.animationClipsNames_.drawArrow, sut_.animationClipsNames_.armed.run.forward});
}
TEST_F(CharacterControllerTests, RecoilRun_AimStopEvent)
{
    prepareState(*this);
    tiggerAndExpect<AimStopEvent, ArmedRunState>({sut_.animationClipsNames_.armed.run.forward});
}
