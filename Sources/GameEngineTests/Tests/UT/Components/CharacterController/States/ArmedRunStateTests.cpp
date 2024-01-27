#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    EXPECT_CALL(test.physicsApiMock_, GetVelocity(test.rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(test.physicsApiMock_, GetRotation(test.rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    test.expectForwardVelocity(DEFAULT_RUN_SPEED);

    test.expectState<DisarmedIdleState>();
    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.idle});
    test.tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {test.sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    test.tiggerAndExpect<RunForwardEvent, ArmedRunState>({test.sut_.animationClipsNames_.armed.run.forward});
}
}  // namespace

TEST_F(CharacterControllerTests, ArmedRun_DrawArrowEvent)
{
    prepareState(*this);
    tiggerAndExpect<DrawArrowEvent, DrawArrowRunState>(
        {sut_.animationClipsNames_.drawArrow, sut_.animationClipsNames_.armed.run.forward});
}
