#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    EXPECT_CALL(test.inputManagerMock_, CalcualteMouseMove()).WillRepeatedly(Return(vec2i{0, 0}));
    EXPECT_CALL(test.physicsApiMock_, GetVelocity(test.rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(test.physicsApiMock_, GetRotation(test.rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    test.expectForwardVelocity(DEFAULT_WALK_SPEED);

    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.posture.stand.idle});
    test.tiggerAndExpect<WeaponStateEvent>(
        {test.sut_.animationClipsNames_.armed.posture.stand.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    test.tiggerAndExpect<DrawArrowEvent>({test.sut_.animationClipsNames_.aim.draw});
    test.Update(ADVANCED_TIME_CLIP_TIME);
    test.Update(ADVANCED_TIME_TRANSITION_TIME);

    test.tiggerAndExpect<AttackEvent>({test.sut_.animationClipsNames_.aim.recoil});
    test.tiggerAndExpect<MoveForwardEvent>(
        {test.sut_.animationClipsNames_.aim.recoil, test.sut_.animationClipsNames_.armed.movement.walk.forward});
}
}  // namespace

TEST_F(CharacterControllerTests, RecoilWalk_CrouchChangeStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<CrouchChangeStateEvent>(
        {sut_.animationClipsNames_.aim.recoil, sut_.animationClipsNames_.armed.movement.crouch.forward});
}
TEST_F(CharacterControllerTests, RecoilWalk_RotateLeft)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));
    tiggerAndExpect<RotateLeftEvent>(
        {sut_.animationClipsNames_.aim.recoil, sut_.animationClipsNames_.armed.movement.walk.forward});
}
TEST_F(CharacterControllerTests, RecoilWalk_RotateRight)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));
    tiggerAndExpect<RotateRightEvent>(
        {sut_.animationClipsNames_.aim.recoil, sut_.animationClipsNames_.armed.movement.walk.forward});
}
TEST_F(CharacterControllerTests, RecoilWalk_RotateTargetEvent)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent>(
        RotateTargetEvent{targetRotation.value_},
        {sut_.animationClipsNames_.aim.recoil, sut_.animationClipsNames_.armed.movement.walk.forward});
}
TEST_F(CharacterControllerTests, RecoilWalk_WeaponStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<WeaponStateEvent>(
        {sut_.animationClipsNames_.disarm, sut_.animationClipsNames_.disarmed.movement.walk.forward});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.movement.walk.forward});
}
TEST_F(CharacterControllerTests, RecoilWalk_ReloadArrowEvent)
{
    prepareState(*this);
    tiggerAndExpect<ReloadArrowEvent>(
        {sut_.animationClipsNames_.aim.draw, sut_.animationClipsNames_.armed.movement.walk.forward});
}
TEST_F(CharacterControllerTests, RecoilWalk_AimStopEvent)
{
    prepareState(*this);
    tiggerAndExpect<AimStopEvent>({sut_.animationClipsNames_.armed.movement.walk.forward});
}
TEST_F(CharacterControllerTests, RecoilWalk_MoveForwardEvent)
{
    prepareState(*this);
    tiggerAndExpect<MoveForwardEvent>(
        {sut_.animationClipsNames_.aim.recoil, sut_.animationClipsNames_.armed.movement.walk.forward});
}
TEST_F(CharacterControllerTests, RecoilWalk_MoveBackwardEvent)
{
    prepareState(*this);
    expectNoMove();
    tiggerAndExpect<MoveBackwardEvent>({sut_.animationClipsNames_.aim.recoil});
}
TEST_F(CharacterControllerTests, RecoilWalk_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent>({sut_.animationClipsNames_.armed.posture.stand.death});
}
TEST_F(CharacterControllerTests, RecoilWalk_SprintStateChangeEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent>({sut_.animationClipsNames_.armed.sprint});
}
TEST_F(CharacterControllerTests, RecoilWalk_WalkChangeStateEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent>(
        {sut_.animationClipsNames_.aim.recoil, sut_.animationClipsNames_.armed.movement.walk.forward});
}
TEST_F(CharacterControllerTests, RecoilWalk_MoveLeftEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_FORWARD + VECTOR_LEFT, vec3(DEFAULT_WALK_LEFT_RIGHT_SPEED, 0.0, DEFAULT_WALK_SPEED));
    tiggerAndExpect<MoveLeftEvent>({sut_.animationClipsNames_.aim.recoil, sut_.animationClipsNames_.armed.movement.walk.forward});
}
TEST_F(CharacterControllerTests, RecoilWalk_RunBackwardEvent)
{
    prepareState(*this);
    expectNoMove();
    tiggerAndExpect<MoveBackwardEvent>({sut_.animationClipsNames_.aim.recoil});
    expectVelocity(VECTOR_BACKWARD, vec3(0.0, 0.0, DEFAULT_BACKWARD_WALK_SPEED));
    tiggerAndExpect<EndForwardMoveEvent>(
        {sut_.animationClipsNames_.aim.recoil, sut_.animationClipsNames_.armed.movement.walk.backward});
}
TEST_F(CharacterControllerTests, RecoilWalk_EndForwardMoveEvent)
{
    prepareState(*this);
    tiggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.aim.recoil});
}
TEST_F(CharacterControllerTests, RecoilWalk_EndBackwardMoveEvent)
{
    prepareState(*this);
    tiggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.aim.recoil});
    expectForwardVelocity(-DEFAULT_BACKWARD_WALK_SPEED);
    tiggerAndExpect<MoveBackwardEvent>(
        {sut_.animationClipsNames_.aim.recoil, sut_.animationClipsNames_.armed.movement.walk.backward});
    tiggerAndExpect<EndBackwardMoveEvent>({sut_.animationClipsNames_.aim.recoil});
}
TEST_F(CharacterControllerTests, RecoilWalk_ReloadArrowEventWhenRunLeftAndRight)
{
    prepareState(*this);
    tiggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.aim.recoil});
    expectVelocity(VECTOR_LEFT, vec3(DEFAULT_WALK_LEFT_RIGHT_SPEED, 0.f, 0.f));
    tiggerAndExpect<MoveLeftEvent>({sut_.animationClipsNames_.aim.recoil, sut_.animationClipsNames_.armed.movement.walk.left});
    expectNoMove();
    tiggerAndExpect<MoveRightEvent>({sut_.animationClipsNames_.aim.recoil});
    expectNoMove();
    tiggerAndExpect<ReloadArrowEvent>({sut_.animationClipsNames_.aim.draw});
}
