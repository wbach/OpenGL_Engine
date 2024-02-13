#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    EXPECT_CALL(test.inputManagerMock_, CalcualteMouseMove()).WillRepeatedly(Return(vec2i{ 0, 0 }));
    EXPECT_CALL(test.physicsApiMock_, SetRotation(test.rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));
    EXPECT_CALL(test.physicsApiMock_, GetRotation(test.rigidbodyid)).WillRepeatedly(Return(Rotation().value_));

    test.expectState<DisarmedIdleState>();
    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.idle});
    test.tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {test.sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    test.tiggerAndExpect<DrawArrowEvent, DrawArrowState>({test.sut_.animationClipsNames_.drawArrow});
    test.tiggerAndExpect<RotateRightEvent, DrawArrowRotateState>(
        {test.sut_.animationClipsNames_.drawArrow, test.sut_.animationClipsNames_.armed.rotateRight});
}
}  // namespace

TEST_F(CharacterControllerTests, DrawArowRotate_RotateLeft)
{
    prepareState(*this);
    tiggerAndExpect<RotateLeftEvent, DrawArrowRotateState>(
        {sut_.animationClipsNames_.drawArrow, sut_.animationClipsNames_.armed.rotateLeft});
}
TEST_F(CharacterControllerTests, DrawArowRotate_RotateRight)
{
    prepareState(*this);
    tiggerAndExpect<RotateRightEvent, DrawArrowRotateState>(
        {sut_.animationClipsNames_.drawArrow, sut_.animationClipsNames_.armed.rotateRight});
}
TEST_F(CharacterControllerTests, DrawArowRotate_RotateTargetEvent)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent, DrawArrowRotateState>(RotateTargetEvent{ targetRotation.value_ },
        {sut_.animationClipsNames_.drawArrow, sut_.animationClipsNames_.armed.rotateLeft});
}
TEST_F(CharacterControllerTests, DrawArowRotate_WeaponStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<WeaponStateEvent, RotateArmedChangeState>(
        {sut_.animationClipsNames_.disarm, sut_.animationClipsNames_.disarmed.rotateRight});

    expectRotationRight(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectRotationRight(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectState<DisarmedRotateState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.rotateRight});

}
TEST_F(CharacterControllerTests, DrawArowRotate_EndRotationEvent)
{
    prepareState(*this);
    tiggerAndExpect<EndRotationEvent, DrawArrowState>({sut_.animationClipsNames_.drawArrow});
}
TEST_F(CharacterControllerTests, DrawArowRotate_AimStopEvent)
{
    prepareState(*this);
    tiggerAndExpect<AimStopEvent, ArmedRotateState>({sut_.animationClipsNames_.armed.rotateRight});
}
TEST_F(CharacterControllerTests, DrawArowRotateToAimRotate)
{
    prepareState(*this);
    tiggerAndExpect<AimStartEvent, AimRotateState>(
        {sut_.animationClipsNames_.armed.rotateRight, sut_.animationClipsNames_.aimIdle});
}
TEST_F(CharacterControllerTests, DrawArowRotate_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent, DeathState>({sut_.animationClipsNames_.armed.death});
}
TEST_F(CharacterControllerTests, DrawArowRotate_SprintStartEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStartEvent, ArmedSprintAndRotateState>({sut_.animationClipsNames_.armed.sprint});
}
TEST_F(CharacterControllerTests, DrawArowRotate_WalkForwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkForwardEvent, DrawArrowWalkAndRotateState>(
        {sut_.animationClipsNames_.drawArrow, sut_.animationClipsNames_.armed.walk.forward});
}
TEST_F(CharacterControllerTests, DrawArowRotate_WalkBackwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(-DEFAULT_BACKWARD_WALK_SPEED);
    tiggerAndExpect<WalkBackwardEvent, DrawArrowWalkAndRotateState>(
        {sut_.animationClipsNames_.drawArrow, sut_.animationClipsNames_.armed.walk.backward});
}
TEST_F(CharacterControllerTests, DrawArowRotate_RunForwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<RunForwardEvent, DrawArrowWalkAndRotateState>(
        {sut_.animationClipsNames_.drawArrow, sut_.animationClipsNames_.armed.walk.forward});
}
TEST_F(CharacterControllerTests, DrawArowRotate_RunBackwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(-DEFAULT_BACKWARD_WALK_SPEED);
    tiggerAndExpect<RunBackwardEvent, DrawArrowWalkAndRotateState>(
        {sut_.animationClipsNames_.drawArrow, sut_.animationClipsNames_.armed.walk.backward});
}
