#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    EXPECT_CALL(test.inputManagerMock_, CalcualteMouseMove()).WillRepeatedly(Return(vec2i{0, 0}));
    EXPECT_CALL(test.physicsApiMock_, SetRotation(test.rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(0));
    EXPECT_CALL(test.physicsApiMock_, GetRotation(test.rigidbodyid)).WillRepeatedly(Return(Rotation().value_));

    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.idle});
    test.tiggerAndExpect<WeaponStateEvent>(
        {test.sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    test.tiggerAndExpect<CrouchChangeStateEvent>({test.sut_.animationClipsNames_.armed.crouchIdle});
    test.tiggerAndExpect<DrawArrowEvent>(
        {test.sut_.animationClipsNames_.drawArrow, test.sut_.animationClipsNames_.armed.crouchIdle});
}
}  // namespace

TEST_F(CharacterControllerTests, DrawArowCrouchState_CrouchChangeStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<CrouchChangeStateEvent>({sut_.animationClipsNames_.drawArrow});
}

// TEST_F(CharacterControllerTests, DrawArowCrouchState_RotateLeft)
//{
//     prepareState(*this);
//     tiggerAndExpect<RotateLeftEvent>({sut_.animationClipsNames_.drawArrow,
//     sut_.animationClipsNames_.armed.rotateLeft});
// }
// TEST_F(CharacterControllerTests, DrawArowCrouchState_RotateRight)
//{
//     prepareState(*this);
//     tiggerAndExpect<RotateRightEvent>(
//         {sut_.animationClipsNames_.drawArrow, sut_.animationClipsNames_.armed.rotateRight});
// }
// TEST_F(CharacterControllerTests, DrawArowCrouchState_RotateTargetEvent)
//{
//     prepareState(*this);
//     EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

//    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
//    tiggerAndExpect<RotateTargetEvent>(
//        RotateTargetEvent{targetRotation.value_},
//        {sut_.animationClipsNames_.drawArrow, sut_.animationClipsNames_.armed.rotateLeft});
//}

TEST_F(CharacterControllerTests, DrawArowCrouchState_WeaponStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<WeaponStateEvent>({sut_.animationClipsNames_.disarm, sut_.animationClipsNames_.armed.crouchIdle});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.crouchIdle});
}
TEST_F(CharacterControllerTests, DrawArowCrouchState_AimStartEvent)
{
    prepareState(*this);
    tiggerAndExpect<AimStartEvent>({sut_.animationClipsNames_.aimIdle, sut_.animationClipsNames_.armed.crouchIdle});
}
TEST_F(CharacterControllerTests, DrawArowCrouchState_AimStopEvent)
{
    prepareState(*this);
    tiggerAndExpect<AimStopEvent>({sut_.animationClipsNames_.armed.crouchIdle});
}
TEST_F(CharacterControllerTests, DrawArowCrouchState_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent>({sut_.animationClipsNames_.armed.death});
}
TEST_F(CharacterControllerTests, DrawArowCrouchState_MoveForwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<MoveForwardEvent>(
        {sut_.animationClipsNames_.armed.crouch.forward, sut_.animationClipsNames_.drawArrow});
}
TEST_F(CharacterControllerTests, DrawArowCrouchState_WalkBackwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(-DEFAULT_BACKWARD_WALK_SPEED);
    tiggerAndExpect<MoveBackwardEvent>(
        {sut_.animationClipsNames_.armed.crouch.backward, sut_.animationClipsNames_.drawArrow});
}
TEST_F(CharacterControllerTests, DrawArowCrouchState_MoveLeftEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_LEFT, vec3(DEFAULT_WALK_LEFT_RIGHT_SPEED, 0.0, 0));
    tiggerAndExpect<MoveLeftEvent>(
        {sut_.animationClipsNames_.armed.crouch.moveleft, sut_.animationClipsNames_.drawArrow});
}
TEST_F(CharacterControllerTests, DrawArowCrouchState_MoveRightEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_RIGHT, vec3(DEFAULT_WALK_LEFT_RIGHT_SPEED, 0.0, 0));
    tiggerAndExpect<MoveRightEvent>(
        {sut_.animationClipsNames_.armed.crouch.moveRight, sut_.animationClipsNames_.drawArrow});
}
