#include "../CharacterControllerTests.h"

namespace
{
//void prepareState(CharacterControllerTests& test)
//{
//    test.tiggerAndExpect<StartFallingEvent>({test.sut_.animationClipsNames_.aim.draw});
//}

TEST_F(CharacterControllerTests, DisarmedFallingStateTests_EndForwardMoveEvent)
{
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    expectForwardVelocity(DEFAULT_RUN_SPEED);

    tiggerAndExpect<MoveForwardEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
    tiggerAndExpect<StartFallingEvent>({sut_.animationClipsNames_.disarmed.falling}, {ADVANCED_TIME_TRANSITION_TIME, 0.2f, ADVANCED_TIME_TRANSITION_TIME});
    tiggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.disarmed.falling});
    expectNoMove();
    tiggerAndExpect<GroundDetectionEvent>({sut_.animationClipsNames_.disarmed.posture.stand.idle});
}
TEST_F(CharacterControllerTests, DisarmedFallingStateTests_DuringForwardMoveEvent)
{
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    expectForwardVelocity(DEFAULT_RUN_SPEED);

    tiggerAndExpect<MoveForwardEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
    tiggerAndExpect<StartFallingEvent>({sut_.animationClipsNames_.disarmed.falling},
                                       {ADVANCED_TIME_TRANSITION_TIME, 0.2f, ADVANCED_TIME_TRANSITION_TIME});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<GroundDetectionEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
}
}  // namespace
