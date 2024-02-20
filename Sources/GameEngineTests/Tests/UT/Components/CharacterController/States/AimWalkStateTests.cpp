#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    EXPECT_CALL(test.inputManagerMock_, CalcualteMouseMove()).WillRepeatedly(Return(vec2i{0, 0}));
    EXPECT_CALL(test.physicsApiMock_, GetVelocity(test.rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(test.physicsApiMock_, GetRotation(test.rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    test.expectForwardVelocity(DEFAULT_WALK_SPEED);

    test.expectState<DisarmedIdleState>();
    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.idle});
    test.tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {test.sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    test.tiggerAndExpect<DrawArrowEvent, DrawArrowState>({test.sut_.animationClipsNames_.drawArrow});
    test.Update(ADVANCED_TIME_CLIP_TIME);
    test.Update(ADVANCED_TIME_TRANSITION_TIME);
    test.expectState<AimState>();
    test.tiggerAndExpect<WalkForwardEvent, AimWalkState>(
        {test.sut_.animationClipsNames_.aimIdle, test.sut_.animationClipsNames_.armed.walk.forward});
}

void prepareAimState(CharacterControllerTests& test)
{
    EXPECT_CALL(test.inputManagerMock_, CalcualteMouseMove()).WillRepeatedly(Return(vec2i{0, 0}));
    EXPECT_CALL(test.physicsApiMock_, GetVelocity(test.rigidbodyid)).WillRepeatedly(Return(vec3(0)));
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

TEST_F(CharacterControllerTests, AimWalk_RotateLeft)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));
    tiggerAndExpect<RotateLeftEvent, AimWalkAndRotateState>(
        {sut_.animationClipsNames_.aimIdle, sut_.animationClipsNames_.armed.walk.forward});
}
TEST_F(CharacterControllerTests, AimWalk_RotateRight)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));
    tiggerAndExpect<RotateRightEvent, AimWalkAndRotateState>(
        {sut_.animationClipsNames_.aimIdle, sut_.animationClipsNames_.armed.walk.forward});
}
TEST_F(CharacterControllerTests, AimWalk_RotateTargetEvent)
{
    prepareState(*this);

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);

    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));
    tiggerAndExpect<RotateTargetEvent, AimWalkAndRotateState>(
        RotateTargetEvent{targetRotation.value_},
        {sut_.animationClipsNames_.aimIdle, sut_.animationClipsNames_.armed.walk.forward});
}
TEST_F(CharacterControllerTests, AimWalk_WeaponStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<WeaponStateEvent, WalkArmedChangeState>(
        {sut_.animationClipsNames_.disarmed.walk.forward, sut_.animationClipsNames_.disarm});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectState<DisarmedWalkState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.walk.forward});
}
TEST_F(CharacterControllerTests, AimWalk_AttackEvent)
{
    prepareState(*this);
    tiggerAndExpect<AttackEvent, RecoilWalkState>(
        {sut_.animationClipsNames_.recoilArrow, sut_.animationClipsNames_.armed.walk.forward});
}
TEST_F(CharacterControllerTests, AimWalk_AimStopEvent)
{
    prepareState(*this);
    tiggerAndExpect<AimStopEvent, ArmedWalkState>({sut_.animationClipsNames_.armed.walk.forward});
}
TEST_F(CharacterControllerTests, AimWalk_RunForwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<RunForwardEvent, AimWalkState>(
        {sut_.animationClipsNames_.aimIdle, sut_.animationClipsNames_.armed.walk.forward});
}
TEST_F(CharacterControllerTests, AimWalk_RunBackwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(-DEFAULT_BACKWARD_WALK_SPEED);
    tiggerAndExpect<RunBackwardEvent, AimWalkState>(
        {sut_.animationClipsNames_.aimIdle, sut_.animationClipsNames_.armed.walk.backward});
}
TEST_F(CharacterControllerTests, AimWalk_WalkChangeStateEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent, AimWalkState>(
        {sut_.animationClipsNames_.aimIdle, sut_.animationClipsNames_.armed.walk.forward});
}
TEST_F(CharacterControllerTests, AimWalk_EndForwardMoveEvent)
{
    prepareState(*this);
    tiggerAndExpect<EndForwardMoveEvent, AimState>({sut_.animationClipsNames_.aimIdle});
}
TEST_F(CharacterControllerTests, AimWalk_EndBackwardMoveEvent)
{
    prepareState(*this);
    tiggerAndExpect<EndForwardMoveEvent, AimState>({sut_.animationClipsNames_.aimIdle});
    expectForwardVelocity(-DEFAULT_BACKWARD_WALK_SPEED);
    tiggerAndExpect<WalkBackwardEvent, AimWalkState>(
        {sut_.animationClipsNames_.aimIdle, sut_.animationClipsNames_.armed.walk.backward});
    tiggerAndExpect<EndBackwardMoveEvent, AimState>({sut_.animationClipsNames_.aimIdle});
}
TEST_F(CharacterControllerTests, AimWalk_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent, DeathState>({sut_.animationClipsNames_.armed.death});
}
TEST_F(CharacterControllerTests, AimWalk_SprintStateChangeEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent, ArmedSprintState>({sut_.animationClipsNames_.armed.sprint});
}
TEST_F(CharacterControllerTests, AimWalk_RunLeftEvent)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_,
                SetVelocityRigidbody(rigidbodyid, vec3(DEFAULT_WALK_LEFT_RIGHT_SPEED, 0.0, DEFAULT_WALK_SPEED)))
        .Times(AtLeast(1));

    DEBUG_LOG("expected Velocity : " + std::to_string(vec3(DEFAULT_WALK_LEFT_RIGHT_SPEED, 0.0, DEFAULT_WALK_SPEED)));
    tiggerAndExpect<RunLeftEvent, AimWalkState>(
        {sut_.animationClipsNames_.armed.walk.moveleft, sut_.animationClipsNames_.aimIdle});
}
TEST_F(CharacterControllerTests, AimWalk_RunRightEvent)
{
    prepareState(*this);
    expectLeftVelocity(-DEFAULT_WALK_LEFT_RIGHT_SPEED);
    tiggerAndExpect<RunRightEvent, AimWalkState>(
        {sut_.animationClipsNames_.armed.walk.moveRight, sut_.animationClipsNames_.aimIdle});
}
TEST_F(CharacterControllerTests, AimWalk_WalkLeftEventEndMoveLeft)
{
    prepareState(*this);
    expectLeftVelocity(DEFAULT_WALK_LEFT_RIGHT_SPEED);
    tiggerAndExpect<WalkLeftEvent, AimWalkState>(
        {sut_.animationClipsNames_.armed.walk.moveleft, sut_.animationClipsNames_.aimIdle});
    tiggerAndExpect<EndMoveLeftEvent, AimWalkState>(
        {sut_.animationClipsNames_.armed.walk.forward, sut_.animationClipsNames_.aimIdle});
}
TEST_F(CharacterControllerTests, AimWalk_WalkRightEventEndMoveRight)
{
    prepareState(*this);
    expectLeftVelocity(-DEFAULT_WALK_LEFT_RIGHT_SPEED);
    tiggerAndExpect<WalkRightEvent, AimWalkState>(
        {sut_.animationClipsNames_.armed.walk.moveRight, sut_.animationClipsNames_.aimIdle});
    tiggerAndExpect<EndMoveLeftEvent, AimWalkState>(
        {sut_.animationClipsNames_.armed.walk.forward, sut_.animationClipsNames_.aimIdle});
}
TEST_F(CharacterControllerTests, AimWalk_EndMoveLeft)
{
    prepareAimState(*this);
    tiggerAndExpect<WalkLeftEvent, AimWalkState>(
        {sut_.animationClipsNames_.armed.walk.moveleft, sut_.animationClipsNames_.aimIdle});
    tiggerAndExpect<EndMoveLeftEvent, AimWalkState>({sut_.animationClipsNames_.aimIdle});
}
TEST_F(CharacterControllerTests, AimWalk_EndMoveRightEvent)
{
    prepareAimState(*this);
    tiggerAndExpect<WalkRightEvent, AimWalkState>(
        {sut_.animationClipsNames_.armed.walk.moveRight, sut_.animationClipsNames_.aimIdle});
    tiggerAndExpect<EndMoveRightEvent, AimWalkState>({sut_.animationClipsNames_.aimIdle});
}
TEST_F(CharacterControllerTests, AimWalk_EndMoveMixed)
{
    prepareAimState(*this);
    tiggerAndExpect<WalkLeftEvent, AimWalkState>(
        {sut_.animationClipsNames_.armed.walk.moveleft, sut_.animationClipsNames_.aimIdle});
    tiggerAndExpect<WalkRightEvent, AimWalkState>(
        {sut_.animationClipsNames_.armed.walk.moveRight, sut_.animationClipsNames_.aimIdle});
    tiggerAndExpect<EndMoveRightEvent, AimWalkState>(
        {sut_.animationClipsNames_.armed.walk.moveleft, sut_.animationClipsNames_.aimIdle});
    tiggerAndExpect<EndMoveLeftEvent, AimWalkState>({sut_.animationClipsNames_.aimIdle});
}
