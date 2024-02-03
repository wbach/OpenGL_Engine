#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    EXPECT_CALL(test.inputManagerMock_, CalcualteMouseMove()).WillRepeatedly(Return(vec2i{0, 0}));
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
    test.tiggerAndExpect<RunForwardEvent, AimRunState>(
        {test.sut_.animationClipsNames_.aimIdle, test.sut_.animationClipsNames_.armed.run.forward});
    test.expectRotationRight(ADVANCED_TIME_TRANSITION_TIME);
    test.tiggerAndExpect<RotateRightEvent, AimRunAndRotateState>(
        {test.sut_.animationClipsNames_.aimIdle, test.sut_.animationClipsNames_.armed.run.forward});
}
}  // namespace

TEST_F(CharacterControllerTests, AimRunAndRotate_RotateLeft)
{
    prepareState(*this);
    expectRotationLeft(ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateLeftEvent, AimRunAndRotateState>(
        {sut_.animationClipsNames_.aimIdle, sut_.animationClipsNames_.armed.run.forward});
}
TEST_F(CharacterControllerTests, AimRunAndRotate_RotateRight)
{
    prepareState(*this);
    tiggerAndExpect<RotateRightEvent, AimRunAndRotateState>(
        {sut_.animationClipsNames_.aimIdle, sut_.animationClipsNames_.armed.run.forward});
}
TEST_F(CharacterControllerTests, AimRunAndRotate_RotateTargetEvent)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent, AimRunAndRotateState>(
        RotateTargetEvent{targetRotation.value_},
        {sut_.animationClipsNames_.aimIdle, sut_.animationClipsNames_.armed.run.forward});
}
TEST_F(CharacterControllerTests, AimRunAndRotate_WeaponStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<WeaponStateEvent, DisarmedRunAndRotateState>(
        {sut_.animationClipsNames_.disarm, sut_.animationClipsNames_.disarmed.run.forward});
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
TEST_F(CharacterControllerTests, AimRunAndRotate_RunForwardEvent)
{
    prepareState(*this);
    tiggerAndExpect<RunForwardEvent, AimRunAndRotateState>(
        {sut_.animationClipsNames_.armed.run.forward, sut_.animationClipsNames_.aimIdle});
}
TEST_F(CharacterControllerTests, AimRunAndRotate_RunBackwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(-DEFAULT_BACKWARD_RUN_SPEED);
    tiggerAndExpect<RunBackwardEvent, AimRunAndRotateState>(
        {sut_.animationClipsNames_.armed.run.backward, sut_.animationClipsNames_.aimIdle});
}
TEST_F(CharacterControllerTests, AimRunAndRotate_WalkChangeStateEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent, AimWalkAndRotateState>(
        {sut_.animationClipsNames_.armed.walk.forward, sut_.animationClipsNames_.aimIdle});
}
TEST_F(CharacterControllerTests, AimRunAndRotate_EndRotationEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<EndRotationEvent, AimRunState>(
        {sut_.animationClipsNames_.armed.run.forward, sut_.animationClipsNames_.aimIdle});
}
TEST_F(CharacterControllerTests, AimRunAndRotate_EndForwardMoveEvent)
{
    prepareState(*this);
    tiggerAndExpect<EndForwardMoveEvent, AimRotateState>(
        {sut_.animationClipsNames_.armed.rotateRight, sut_.animationClipsNames_.aimIdle});
}
TEST_F(CharacterControllerTests, AimRunAndRotate_EndBackwardMoveEvent)
{
    prepareState(*this);
    expectForwardVelocity(-DEFAULT_BACKWARD_RUN_SPEED);
    tiggerAndExpect<RunBackwardEvent, AimRunAndRotateState>(
        {sut_.animationClipsNames_.armed.run.backward, sut_.animationClipsNames_.aimIdle});
    tiggerAndExpect<EndBackwardMoveEvent, AimRunAndRotateState>(
        {sut_.animationClipsNames_.armed.run.forward, sut_.animationClipsNames_.aimIdle});
}
TEST_F(CharacterControllerTests, AimRunAndRotate_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent, DeathState>({sut_.animationClipsNames_.armed.death});
}
TEST_F(CharacterControllerTests, AimRunAndRotate_SprintStateChangeEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent, ArmedSprintAndRotateState>({sut_.animationClipsNames_.armed.sprint});
}
