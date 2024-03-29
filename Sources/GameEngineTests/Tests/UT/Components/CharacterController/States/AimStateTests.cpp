#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    EXPECT_CALL(test.inputManagerMock_, CalcualteMouseMove()).WillRepeatedly(Return(vec2i{0, 0}));
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
}
}  // namespace

TEST_F(CharacterControllerTests, Aim_RotateLeft)
{
    prepareState(*this);
    expectRotationLeft();
    tiggerAndExpect<RotateLeftEvent, AimRotateState>(
        {sut_.animationClipsNames_.aimIdle, sut_.animationClipsNames_.armed.rotateLeft});
}
TEST_F(CharacterControllerTests, Aim_RotateRight)
{
    prepareState(*this);
    expectRotationRight();
    tiggerAndExpect<RotateRightEvent, AimRotateState>(
        {sut_.animationClipsNames_.aimIdle, sut_.animationClipsNames_.armed.rotateRight});
}
TEST_F(CharacterControllerTests, Aim_RotateTargetEvent)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent, AimRotateState>(
        RotateTargetEvent{targetRotation.value_},
        {sut_.animationClipsNames_.aimIdle, sut_.animationClipsNames_.armed.rotateLeft});
}
TEST_F(CharacterControllerTests, Aim_WeaponStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<WeaponStateEvent, IdleArmedChangeState>({sut_.animationClipsNames_.disarm});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
}
TEST_F(CharacterControllerTests, Aim_AttackEvent)
{
    prepareState(*this);
    tiggerAndExpect<AttackEvent, RecoilState>({sut_.animationClipsNames_.recoilArrow});
}
TEST_F(CharacterControllerTests, Aim_AimStopEvent)
{
    prepareState(*this);
    tiggerAndExpect<AimStopEvent, ArmedIdleState>({sut_.animationClipsNames_.armed.idle});
}
TEST_F(CharacterControllerTests, Aim_RunForwardEvent)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<RunForwardEvent, AimWalkState>(
        {sut_.animationClipsNames_.aimIdle, sut_.animationClipsNames_.armed.walk.forward});
}
TEST_F(CharacterControllerTests, Aim_RunBackwardEvent)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    expectForwardVelocity(-DEFAULT_BACKWARD_WALK_SPEED);
    tiggerAndExpect<RunBackwardEvent, AimWalkState>(
        {sut_.animationClipsNames_.aimIdle, sut_.animationClipsNames_.armed.walk.backward});
}
TEST_F(CharacterControllerTests, Aim_WalkForwardEvent)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkForwardEvent, AimWalkState>(
        {sut_.animationClipsNames_.aimIdle, sut_.animationClipsNames_.armed.walk.forward});
}
TEST_F(CharacterControllerTests, Aim_WalkBackwardEvent)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    expectForwardVelocity(-DEFAULT_BACKWARD_WALK_SPEED);
    tiggerAndExpect<WalkBackwardEvent, AimWalkState>(
        {sut_.animationClipsNames_.aimIdle, sut_.animationClipsNames_.armed.walk.backward});
}
TEST_F(CharacterControllerTests, Aim_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent, DeathState>({sut_.animationClipsNames_.armed.death});
}
TEST_F(CharacterControllerTests, Aim_SprintStartEvent)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStartEvent, ArmedSprintState>({sut_.animationClipsNames_.armed.sprint});
}
TEST_F(CharacterControllerTests, Aim_RunLeftEvent)
{
    prepareState(*this);
    expectLeftVelocity(DEFAULT_WALK_LEFT_RIGHT_SPEED);
    tiggerAndExpect<RunLeftEvent, AimWalkState>(
        {sut_.animationClipsNames_.armed.walk.moveleft, sut_.animationClipsNames_.aimIdle});
}
TEST_F(CharacterControllerTests, Aim_RunRightEvent)
{
    prepareState(*this);
    expectLeftVelocity(-DEFAULT_WALK_LEFT_RIGHT_SPEED);
    tiggerAndExpect<RunRightEvent, AimWalkState>(
        {sut_.animationClipsNames_.armed.walk.moveRight, sut_.animationClipsNames_.aimIdle});
}
TEST_F(CharacterControllerTests, Aim_WalkLeftEvent)
{
    prepareState(*this);
    expectLeftVelocity(DEFAULT_WALK_LEFT_RIGHT_SPEED);
    tiggerAndExpect<WalkLeftEvent, AimWalkState>(
        {sut_.animationClipsNames_.armed.walk.moveleft, sut_.animationClipsNames_.aimIdle});
}
TEST_F(CharacterControllerTests, Aim_WalkRightEvent)
{
    prepareState(*this);
    expectLeftVelocity(-DEFAULT_WALK_LEFT_RIGHT_SPEED);
    tiggerAndExpect<WalkRightEvent, AimWalkState>(
        {sut_.animationClipsNames_.armed.walk.moveRight, sut_.animationClipsNames_.aimIdle});
}
