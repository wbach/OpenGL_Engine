#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    EXPECT_CALL(test.inputManagerMock_, CalcualteMouseMove()).WillRepeatedly(Return(vec2i{0, 0}));
    EXPECT_CALL(test.physicsApiMock_, GetVelocity(test.rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(test.physicsApiMock_, GetRotation(test.rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    test.expectForwardVelocity(DEFAULT_WALK_SPEED);

    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.idle});
    test.tiggerAndExpect<WeaponStateEvent>(
        {test.sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    test.tiggerAndExpect<DrawArrowEvent>({test.sut_.animationClipsNames_.drawArrow});
    test.Update(ADVANCED_TIME_CLIP_TIME);
    test.Update(ADVANCED_TIME_TRANSITION_TIME);
    test.tiggerAndExpect<MoveForwardEvent>(
        {test.sut_.animationClipsNames_.aimIdle, test.sut_.animationClipsNames_.armed.walk.forward});
}

void prepareAimState(CharacterControllerTests& test)
{
    EXPECT_CALL(test.inputManagerMock_, CalcualteMouseMove()).WillRepeatedly(Return(vec2i{0, 0}));
    EXPECT_CALL(test.physicsApiMock_, GetVelocity(test.rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(test.physicsApiMock_, GetRotation(test.rigidbodyid)).WillRepeatedly(Return(Rotation().value_));

    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.idle});
    test.tiggerAndExpect<WeaponStateEvent>(
        {test.sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    test.tiggerAndExpect<DrawArrowEvent>({test.sut_.animationClipsNames_.drawArrow});
    test.Update(ADVANCED_TIME_CLIP_TIME);
    test.Update(ADVANCED_TIME_TRANSITION_TIME);
}
}  // namespace

TEST_F(CharacterControllerTests, AimWalk_RotateLeft)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));
    tiggerAndExpect<RotateLeftEvent>({sut_.animationClipsNames_.aimIdle, sut_.animationClipsNames_.armed.walk.forward});
}
TEST_F(CharacterControllerTests, AimWalk_RotateRight)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));
    tiggerAndExpect<RotateRightEvent>(
        {sut_.animationClipsNames_.aimIdle, sut_.animationClipsNames_.armed.walk.forward});
}
TEST_F(CharacterControllerTests, AimWalk_RotateTargetEvent)
{
    prepareState(*this);

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);

    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));
    tiggerAndExpect<RotateTargetEvent>(
        RotateTargetEvent{targetRotation.value_},
        {sut_.animationClipsNames_.aimIdle, sut_.animationClipsNames_.armed.walk.forward});
}
TEST_F(CharacterControllerTests, AimWalk_WeaponStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<WeaponStateEvent>(
        {sut_.animationClipsNames_.disarmed.walk.forward, sut_.animationClipsNames_.disarm});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.walk.forward});
}
TEST_F(CharacterControllerTests, AimWalk_AttackEvent)
{
    prepareState(*this);
    tiggerAndExpect<AttackEvent>({sut_.animationClipsNames_.recoilArrow, sut_.animationClipsNames_.armed.walk.forward});
}
TEST_F(CharacterControllerTests, AimWalk_AimStopEvent)
{
    prepareState(*this);
    tiggerAndExpect<AimStopEvent>({sut_.animationClipsNames_.armed.walk.forward});
}
TEST_F(CharacterControllerTests, AimWalk_MoveForwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<MoveForwardEvent>(
        {sut_.animationClipsNames_.aimIdle, sut_.animationClipsNames_.armed.walk.forward});
}
TEST_F(CharacterControllerTests, AimWalk_RunBackwardEvent)
{
    prepareState(*this);
    expectNoMove();
    tiggerAndExpect<MoveBackwardEvent>({sut_.animationClipsNames_.aimIdle});
}
TEST_F(CharacterControllerTests, AimWalk_WalkChangeStateEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent>(
        {sut_.animationClipsNames_.aimIdle, sut_.animationClipsNames_.armed.walk.forward});
}
TEST_F(CharacterControllerTests, AimWalk_EndForwardMoveEvent)
{
    prepareState(*this);
    tiggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.aimIdle});
}
TEST_F(CharacterControllerTests, AimWalk_EndBackwardMoveEvent)
{
    prepareState(*this);
    tiggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.aimIdle});
    expectForwardVelocity(-DEFAULT_BACKWARD_WALK_SPEED);
    tiggerAndExpect<MoveBackwardEvent>(
        {sut_.animationClipsNames_.aimIdle, sut_.animationClipsNames_.armed.walk.backward});
    tiggerAndExpect<EndBackwardMoveEvent>({sut_.animationClipsNames_.aimIdle});
}
TEST_F(CharacterControllerTests, AimWalk_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent>({sut_.animationClipsNames_.armed.death});
}
TEST_F(CharacterControllerTests, AimWalk_SprintStateChangeEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent>({sut_.animationClipsNames_.armed.sprint});
}
TEST_F(CharacterControllerTests, AimWalk_MoveLeftEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_FORWARD + VECTOR_LEFT, vec3(DEFAULT_WALK_LEFT_RIGHT_SPEED, 0.0, DEFAULT_WALK_SPEED));
    tiggerAndExpect<MoveLeftEvent>({sut_.animationClipsNames_.armed.walk.forward, sut_.animationClipsNames_.aimIdle});
}
TEST_F(CharacterControllerTests, AimWalk_MoveRightEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_FORWARD + VECTOR_RIGHT, vec3(DEFAULT_WALK_LEFT_RIGHT_SPEED, 0.0, DEFAULT_WALK_SPEED));
    tiggerAndExpect<MoveRightEvent>({sut_.animationClipsNames_.armed.walk.forward, sut_.animationClipsNames_.aimIdle});
}
TEST_F(CharacterControllerTests, AimWalk_MoveLeftEventEndMoveLeft)
{
    prepareState(*this);
    expectVelocity(VECTOR_FORWARD + VECTOR_LEFT, vec3(DEFAULT_WALK_LEFT_RIGHT_SPEED, 0.0, DEFAULT_WALK_SPEED));
    tiggerAndExpect<MoveLeftEvent>({sut_.animationClipsNames_.armed.walk.forward, sut_.animationClipsNames_.aimIdle});
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<EndMoveLeftEvent>(
        {sut_.animationClipsNames_.armed.walk.forward, sut_.animationClipsNames_.aimIdle});
}
TEST_F(CharacterControllerTests, AimWalk_MoveRightEventEndMoveRight)
{
    prepareState(*this);
    expectVelocity(VECTOR_FORWARD + VECTOR_RIGHT, vec3(DEFAULT_WALK_LEFT_RIGHT_SPEED, 0.0, DEFAULT_WALK_SPEED));
    tiggerAndExpect<MoveRightEvent>({sut_.animationClipsNames_.armed.walk.forward, sut_.animationClipsNames_.aimIdle});
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<EndMoveRightEvent>(
        {sut_.animationClipsNames_.armed.walk.forward, sut_.animationClipsNames_.aimIdle});
}
TEST_F(CharacterControllerTests, AimWalk_EndMoveLeft)
{
    prepareAimState(*this);
    expectVelocity(VECTOR_LEFT, vec3(DEFAULT_WALK_LEFT_RIGHT_SPEED, 0.0, 0));
    tiggerAndExpect<MoveLeftEvent>({sut_.animationClipsNames_.armed.walk.moveleft, sut_.animationClipsNames_.aimIdle});
    expectNoMove();
    tiggerAndExpect<EndMoveLeftEvent>({sut_.animationClipsNames_.aimIdle});
}
TEST_F(CharacterControllerTests, AimWalk_EndMoveRightEvent)
{
    prepareAimState(*this);
    expectVelocity(VECTOR_RIGHT, vec3(DEFAULT_WALK_LEFT_RIGHT_SPEED, 0.0, 0));
    tiggerAndExpect<MoveRightEvent>(
        {sut_.animationClipsNames_.armed.walk.moveRight, sut_.animationClipsNames_.aimIdle});
    expectNoMove();
    tiggerAndExpect<EndMoveRightEvent>({sut_.animationClipsNames_.aimIdle});
}
TEST_F(CharacterControllerTests, AimWalk_EndMoveMixed)
{
    prepareAimState(*this);
    expectVelocity(VECTOR_LEFT, vec3(DEFAULT_WALK_LEFT_RIGHT_SPEED, 0.0, 0));
    tiggerAndExpect<MoveLeftEvent>({sut_.animationClipsNames_.armed.walk.moveleft, sut_.animationClipsNames_.aimIdle});
    expectNoMove();
    tiggerAndExpect<MoveRightEvent>({sut_.animationClipsNames_.aimIdle});
    expectVelocity(VECTOR_RIGHT, vec3(DEFAULT_WALK_LEFT_RIGHT_SPEED, 0.0, 0.0));
    tiggerAndExpect<EndMoveLeftEvent>(
        {sut_.animationClipsNames_.armed.walk.moveRight, sut_.animationClipsNames_.aimIdle});
    expectNoMove();
    tiggerAndExpect<EndMoveRightEvent>({sut_.animationClipsNames_.aimIdle});
}
TEST_F(CharacterControllerTests, AimWalk_RunEndMoveLeft)
{
    prepareState(*this);
    expectVelocity(VECTOR_LEFT + VECTOR_FORWARD, vec3(DEFAULT_WALK_LEFT_RIGHT_SPEED, 0.0, DEFAULT_WALK_SPEED));
    tiggerAndExpect<MoveLeftEvent>({sut_.animationClipsNames_.armed.walk.forward, sut_.animationClipsNames_.aimIdle});
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<EndMoveLeftEvent>(
        {sut_.animationClipsNames_.armed.walk.forward, sut_.animationClipsNames_.aimIdle});
    expectNoMove();
    tiggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.aimIdle});
}
TEST_F(CharacterControllerTests, AimWalk_MoveEndForwardWhenMoveLeft)
{
    prepareState(*this);
    expectVelocity(VECTOR_LEFT + VECTOR_FORWARD, vec3(DEFAULT_WALK_LEFT_RIGHT_SPEED, 0.0, DEFAULT_WALK_SPEED));
    tiggerAndExpect<MoveLeftEvent>({sut_.animationClipsNames_.armed.walk.forward, sut_.animationClipsNames_.aimIdle});
    expectLeftVelocity(DEFAULT_WALK_LEFT_RIGHT_SPEED);
    tiggerAndExpect<EndForwardMoveEvent>(
        {sut_.animationClipsNames_.armed.walk.moveleft, sut_.animationClipsNames_.aimIdle});
    expectNoMove();
    tiggerAndExpect<EndMoveLeftEvent>({sut_.animationClipsNames_.aimIdle});
}
TEST_F(CharacterControllerTests, AimWalk_AimStopEventWhenMoveLeft)
{
    prepareAimState(*this);
    expectVelocity(VECTOR_LEFT, vec3(DEFAULT_WALK_LEFT_RIGHT_SPEED, 0.0, 0.0));
    tiggerAndExpect<MoveLeftEvent>({sut_.animationClipsNames_.armed.walk.moveleft, sut_.animationClipsNames_.aimIdle});
    expectVelocity(VECTOR_LEFT, vec3(DEFAULT_WALK_SPEED, 0.0, 0.0));
    tiggerAndExpect<AimStopEvent>({sut_.animationClipsNames_.armed.walk.forward});
    expectRootboneRotation(VECTOR_LEFT);
    expectNoMove();
    tiggerAndExpect<EndMoveLeftEvent>({sut_.animationClipsNames_.armed.idle});
}
