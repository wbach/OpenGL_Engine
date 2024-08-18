#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    EXPECT_CALL(test.inputManagerMock_, CalcualteMouseMove()).WillRepeatedly(Return(vec2i{0, 0}));
    // EXPECT_CALL(test.physicsApiMock_, SetRotation(test.rigidbodyid, Matcher<const
    // Quaternion&>(_))).Times(AtLeast(1));
    EXPECT_CALL(test.physicsApiMock_, GetRotation(test.rigidbodyid)).WillRepeatedly(Return(Rotation().value_));

    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.posture.stand.idle});
    test.tiggerAndExpect<WeaponStateEvent>(
        {test.sut_.animationClipsNames_.armed.posture.stand.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    test.tiggerAndExpect<DrawArrowEvent>({test.sut_.animationClipsNames_.aim.draw});
    test.Update(ADVANCED_TIME_CLIP_TIME);
    test.Update(ADVANCED_TIME_TRANSITION_TIME);

    test.expectRotationLeft(ADVANCED_TIME_TRANSITION_TIME);
    test.tiggerAndExpect<RotateLeftEvent>(
        {test.sut_.animationClipsNames_.aim.idle, test.sut_.animationClipsNames_.armed.posture.stand.rotate.left});
}
}  // namespace

TEST_F(CharacterControllerTests, AimRotate_CrouchChangeStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<CrouchChangeStateEvent>(
        {sut_.animationClipsNames_.aim.idle, sut_.animationClipsNames_.armed.posture.crouched.rotate.left});
}
TEST_F(CharacterControllerTests, AimRotate_RotateLeft)
{
    prepareState(*this);
    tiggerAndExpect<RotateLeftEvent>(
        {sut_.animationClipsNames_.aim.idle, sut_.animationClipsNames_.armed.posture.stand.rotate.left});
}
TEST_F(CharacterControllerTests, AimRotate_RotateRight)
{
    prepareState(*this);
    expectRotationRight(ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateRightEvent>(
        {sut_.animationClipsNames_.aim.idle, sut_.animationClipsNames_.armed.posture.stand.rotate.right});
}
TEST_F(CharacterControllerTests, AimRotate_RotateTargetEvent)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent>(
        RotateTargetEvent{targetRotation.value_},
        {sut_.animationClipsNames_.aim.idle, sut_.animationClipsNames_.armed.posture.stand.rotate.left});
}
TEST_F(CharacterControllerTests, AimRotate_WeaponStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<WeaponStateEvent>(
        {sut_.animationClipsNames_.disarm, sut_.animationClipsNames_.disarmed.posture.stand.rotate.left});

    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectRotationLeft(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.rotate.left});
}
TEST_F(CharacterControllerTests, AimRotate_AttackEvent)
{
    prepareState(*this);
    tiggerAndExpect<AttackEvent>(
        {sut_.animationClipsNames_.armed.posture.stand.rotate.left, sut_.animationClipsNames_.aim.recoil});
}
TEST_F(CharacterControllerTests, AimRotate_EndRotationEvent)
{
    prepareState(*this);
    tiggerAndExpect<EndRotationEvent>({sut_.animationClipsNames_.aim.idle});
}
TEST_F(CharacterControllerTests, AimRotate_AimStopEvent)
{
    prepareState(*this);
    tiggerAndExpect<AimStopEvent>({sut_.animationClipsNames_.armed.posture.stand.rotate.left});
}
TEST_F(CharacterControllerTests, AimRotate_MoveForwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<MoveForwardEvent>(
        {sut_.animationClipsNames_.armed.movement.walk.forward, sut_.animationClipsNames_.aim.idle});
}
TEST_F(CharacterControllerTests, AimRotate_MoveBackwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(-DEFAULT_BACKWARD_WALK_SPEED);
    tiggerAndExpect<MoveBackwardEvent>(
        {sut_.animationClipsNames_.armed.movement.walk.backward, sut_.animationClipsNames_.aim.idle});
}
TEST_F(CharacterControllerTests, AimRotate_MoveLeftEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_LEFT, vec3(DEFAULT_WALK_LEFT_RIGHT_SPEED, 0.f, 0.f));
    tiggerAndExpect<MoveLeftEvent>({sut_.animationClipsNames_.armed.movement.walk.left, sut_.animationClipsNames_.aim.idle});
}
TEST_F(CharacterControllerTests, AimRotate_MoveRightEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_RIGHT, vec3(DEFAULT_WALK_LEFT_RIGHT_SPEED, 0.f, 0.f));
    tiggerAndExpect<MoveRightEvent>({sut_.animationClipsNames_.armed.movement.walk.right, sut_.animationClipsNames_.aim.idle});
}
TEST_F(CharacterControllerTests, AimRotate_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent>({sut_.animationClipsNames_.armed.posture.stand.death});
}
