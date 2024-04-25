#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    EXPECT_CALL(test.inputManagerMock_, CalcualteMouseMove()).WillRepeatedly(Return(vec2i{0, 0}));
    EXPECT_CALL(test.physicsApiMock_, SetRotation(test.rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));
    EXPECT_CALL(test.physicsApiMock_, GetRotation(test.rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(test.physicsApiMock_, GetVelocity(test.rigidbodyid)).WillRepeatedly(Return(vec3(0)));
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
    test.tiggerAndExpect<RotateRightEvent>(
        {test.sut_.animationClipsNames_.aimIdle, test.sut_.animationClipsNames_.armed.walk.forward});
}
}  // namespace

TEST_F(CharacterControllerTests, AimWalkAndRotate_RotateLeft)
{
    prepareState(*this);
    tiggerAndExpect<RotateLeftEvent>({sut_.animationClipsNames_.aimIdle, sut_.animationClipsNames_.armed.walk.forward});
}
TEST_F(CharacterControllerTests, AimWalkAndRotate_RotateRight)
{
    prepareState(*this);
    tiggerAndExpect<RotateRightEvent>(
        {sut_.animationClipsNames_.aimIdle, sut_.animationClipsNames_.armed.walk.forward});
}
TEST_F(CharacterControllerTests, AimWalkAndRotate_RotateTargetEvent)
{
    prepareState(*this);

    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent>(
        RotateTargetEvent{targetRotation.value_},
        {sut_.animationClipsNames_.aimIdle, sut_.animationClipsNames_.armed.walk.forward});
}
TEST_F(CharacterControllerTests, AimWalkAndRotate_WeaponStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<WeaponStateEvent>(
        {sut_.animationClipsNames_.disarm, sut_.animationClipsNames_.disarmed.walk.forward});

    expectRotationRight(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectRotationRight(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.walk.forward});
}
TEST_F(CharacterControllerTests, AimWalkAndRotate_AttackEvent)
{
    prepareState(*this);
    tiggerAndExpect<AttackEvent>({sut_.animationClipsNames_.recoilArrow, sut_.animationClipsNames_.armed.walk.forward});
}
TEST_F(CharacterControllerTests, AimWalkAndRotate_AimStopEvent)
{
    prepareState(*this);
    tiggerAndExpect<AimStopEvent>({sut_.animationClipsNames_.armed.walk.forward});
}
TEST_F(CharacterControllerTests, AimWalkAndRotate_EndRotationEvent)
{
    prepareState(*this);
    tiggerAndExpect<EndRotationEvent>(
        {sut_.animationClipsNames_.armed.walk.forward, sut_.animationClipsNames_.aimIdle});
}
TEST_F(CharacterControllerTests, AimWalkAndRotate_MoveForwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<MoveForwardEvent>(
        {sut_.animationClipsNames_.armed.walk.forward, sut_.animationClipsNames_.aimIdle});
}
TEST_F(CharacterControllerTests, AimWalkAndRotate_MoveBackwardEvent)
{
    prepareState(*this);
    expectNoMove();
    tiggerAndExpect<MoveBackwardEvent>(
        {sut_.animationClipsNames_.armed.rotateRight, sut_.animationClipsNames_.aimIdle});
}
TEST_F(CharacterControllerTests, AimWalkAndRotate_WalkChangeStateEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent>(
        {sut_.animationClipsNames_.armed.walk.forward, sut_.animationClipsNames_.aimIdle});
}
TEST_F(CharacterControllerTests, AimWalkAndRotate_EndForwardMoveEvent)
{
    prepareState(*this);
    tiggerAndExpect<EndForwardMoveEvent>(
        {sut_.animationClipsNames_.armed.rotateRight, sut_.animationClipsNames_.aimIdle});
}
TEST_F(CharacterControllerTests, AimWalkAndRotate_EndBackwardMoveEvent)
{
    prepareState(*this);
    tiggerAndExpect<EndForwardMoveEvent>(
        {sut_.animationClipsNames_.armed.rotateRight, sut_.animationClipsNames_.aimIdle},
        {ADVANCED_TIME_CLIP_TIME / 4.f});
    expectForwardVelocity(-DEFAULT_BACKWARD_WALK_SPEED);
    tiggerAndExpect<MoveBackwardEvent>(
        {sut_.animationClipsNames_.armed.walk.backward, sut_.animationClipsNames_.aimIdle});
    tiggerAndExpect<EndBackwardMoveEvent>(
        {sut_.animationClipsNames_.armed.rotateRight, sut_.animationClipsNames_.aimIdle},
        {ADVANCED_TIME_CLIP_TIME / 4.f});
}
TEST_F(CharacterControllerTests, AimWalkAndRotate_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent>({sut_.animationClipsNames_.armed.death});
}
TEST_F(CharacterControllerTests, AimWalkAndRotate_SprintStateChangeEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent>({sut_.animationClipsNames_.armed.sprint});
}
