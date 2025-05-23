#include "../CharacterControllerTests.h"

namespace
{
void prepareStateDuringRun(CharacterControllerTests& test)
{
    EXPECT_CALL(test.inputManagerMock_, CalcualteMouseMove()).WillRepeatedly(Return(vec2i{0, 0}));
    EXPECT_CALL(test.physicsApiMock_, GetVelocity(test.rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(test.physicsApiMock_, GetRotation(test.rigidbodyid)).WillRepeatedly(Return(Rotation().value_));

    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.posture.stand.idle});
    test.expectForwardVelocity(DEFAULT_RUN_SPEED);
    test.tiggerAndExpect<MoveForwardEvent>({test.sut_.animationClipsNames_.disarmed.movement.run.forward});
    test.tiggerAndExpect<DodgeDiveEvent>({test.sut_.animationClipsNames_.disarmed.dodgeDive});
    test.Update(ADVANCED_TIME_TRANSITION_TIME);
}
void prepareStateDuringIdle(CharacterControllerTests& test)
{
    EXPECT_CALL(test.inputManagerMock_, CalcualteMouseMove()).WillRepeatedly(Return(vec2i{0, 0}));

    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.posture.stand.idle});
    test.tiggerAndExpect<DodgeDiveEvent>({test.sut_.animationClipsNames_.disarmed.dodgeDive});
    test.Update(ADVANCED_TIME_TRANSITION_TIME);
}
}  // namespace

TEST_F(CharacterControllerTests, DodgeStateTest_Falling)
{
    prepareStateDuringRun(*this);

    handleEvent(StartFallingEvent{});
    Update(ADVANCED_TIME_CLIP_TIME);
    tiggerAndExpect<GroundDetectionEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
}

TEST_F(CharacterControllerTests, DodgeStateTest_BackToIdle)
{
    prepareStateDuringIdle(*this);

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
}
