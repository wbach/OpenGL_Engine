#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    EXPECT_CALL(test.inputManagerMock_, CalcualteMouseMove()).WillRepeatedly(Return(vec2i{0, 0}));
    // EXPECT_CALL(test.physicsApiMock_, SetRotation(test.rigidbodyid, Matcher<const
    // Quaternion&>(_))).Times(AtLeast(1));
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
    test.expectRotationLeft(ADVANCED_TIME_TRANSITION_TIME);
    test.tiggerAndExpect<RotateLeftEvent, AimRotateState>(
        {test.sut_.animationClipsNames_.aimIdle, test.sut_.animationClipsNames_.armed.rotateLeft});
}
}  // namespace

TEST_F(CharacterControllerTests, AimRotate_RotateLeft)
{
    prepareState(*this);
    tiggerAndExpect<RotateLeftEvent, AimRotateState>(
        {sut_.animationClipsNames_.aimIdle, sut_.animationClipsNames_.armed.rotateLeft});
}
TEST_F(CharacterControllerTests, AimRotate_RotateRight)
{
    prepareState(*this);
    expectRotationRight(ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateRightEvent, AimRotateState>(
        {sut_.animationClipsNames_.aimIdle, sut_.animationClipsNames_.armed.rotateRight});
}
TEST_F(CharacterControllerTests, AimRotate_RotateTargetEvent)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent, AimRotateState>(RotateTargetEvent{ targetRotation.value_ },
        { sut_.animationClipsNames_.aimIdle, sut_.animationClipsNames_.armed.rotateLeft });
}
TEST_F(CharacterControllerTests, AimRotate_WeaponStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<WeaponStateEvent, RotateArmedChangeState>(
        {sut_.animationClipsNames_.disarm, sut_.animationClipsNames_.disarmed.rotateLeft});

    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectRotationLeft(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectState<DisarmedRotateState>();
    expectAnimsToBeSet({ sut_.animationClipsNames_.disarmed.rotateLeft });
}
TEST_F(CharacterControllerTests, AimRotate_AttackEvent)
{
    prepareState(*this);
    tiggerAndExpect<AttackEvent, RecoilRotateState>(
        {sut_.animationClipsNames_.armed.rotateLeft, sut_.animationClipsNames_.recoilArrow});
}
TEST_F(CharacterControllerTests, AimRotate_EndRotationEvent)
{
    prepareState(*this);
    tiggerAndExpect<EndRotationEvent, AimState>({sut_.animationClipsNames_.aimIdle});
}
TEST_F(CharacterControllerTests, AimRotate_AimStopEvent)
{
    prepareState(*this);
    tiggerAndExpect<AimStopEvent, ArmedRotateState>({sut_.animationClipsNames_.armed.rotateLeft});
}
TEST_F(CharacterControllerTests, AimRotate_WalkForwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkForwardEvent, AimWalkAndRotateState>(
        {sut_.animationClipsNames_.armed.walk.forward, sut_.animationClipsNames_.aimIdle});
}
TEST_F(CharacterControllerTests, AimRotate_WalkBackwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(-DEFAULT_BACKWARD_WALK_SPEED);
    tiggerAndExpect<WalkBackwardEvent, AimWalkAndRotateState>(
        {sut_.animationClipsNames_.armed.walk.backward, sut_.animationClipsNames_.aimIdle});
}
TEST_F(CharacterControllerTests, AimRotate_RunForwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<RunForwardEvent, AimWalkAndRotateState>(
        {sut_.animationClipsNames_.armed.walk.forward, sut_.animationClipsNames_.aimIdle});
}
TEST_F(CharacterControllerTests, AimRotate_RunBackwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(-DEFAULT_BACKWARD_WALK_SPEED);
    tiggerAndExpect<RunBackwardEvent, AimWalkAndRotateState>(
        {sut_.animationClipsNames_.armed.walk.backward, sut_.animationClipsNames_.aimIdle});
}
TEST_F(CharacterControllerTests, AimRotate_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent, DeathState>({sut_.animationClipsNames_.armed.death});
}
TEST_F(CharacterControllerTests, AimRotate_SprintStartEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStartEvent, ArmedSprintAndRotateState>({sut_.animationClipsNames_.armed.sprint});
}
