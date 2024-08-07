#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    EXPECT_CALL(test.inputManagerMock_, CalcualteMouseMove()).WillRepeatedly(Return(vec2i{0, 0}));
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

TEST_F(CharacterControllerTests, Aim_CrouchChangeStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<CrouchChangeStateEvent>(
        {sut_.animationClipsNames_.aimIdle, sut_.animationClipsNames_.armed.crouchIdle});
}
TEST_F(CharacterControllerTests, Aim_RotateLeft)
{
    prepareState(*this);
    expectRotationLeft();
    tiggerAndExpect<RotateLeftEvent>({sut_.animationClipsNames_.aimIdle, sut_.animationClipsNames_.armed.rotateLeft});
}
TEST_F(CharacterControllerTests, Aim_RotateRight)
{
    prepareState(*this);
    expectRotationRight();
    tiggerAndExpect<RotateRightEvent>({sut_.animationClipsNames_.aimIdle, sut_.animationClipsNames_.armed.rotateRight});
}
TEST_F(CharacterControllerTests, Aim_RotateTargetEvent)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent>(RotateTargetEvent{targetRotation.value_},
                                       {sut_.animationClipsNames_.aimIdle, sut_.animationClipsNames_.armed.rotateLeft});
}
TEST_F(CharacterControllerTests, Aim_WeaponStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<WeaponStateEvent>({sut_.animationClipsNames_.disarm});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
}
TEST_F(CharacterControllerTests, Aim_AttackEvent)
{
    prepareState(*this);
    tiggerAndExpect<AttackEvent>({sut_.animationClipsNames_.recoilArrow});
}
TEST_F(CharacterControllerTests, Aim_AimStopEvent)
{
    prepareState(*this);
    tiggerAndExpect<AimStopEvent>({sut_.animationClipsNames_.armed.idle});
}
TEST_F(CharacterControllerTests, Aim_MoveForwardEvent)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<MoveForwardEvent>(
        {sut_.animationClipsNames_.aimIdle, sut_.animationClipsNames_.armed.walk.forward});
}
TEST_F(CharacterControllerTests, Aim_RunBackwardEvent)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    expectForwardVelocity(-DEFAULT_BACKWARD_WALK_SPEED);
    tiggerAndExpect<MoveBackwardEvent>(
        {sut_.animationClipsNames_.aimIdle, sut_.animationClipsNames_.armed.walk.backward});
}
TEST_F(CharacterControllerTests, Aim_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent>({sut_.animationClipsNames_.armed.death});
}
TEST_F(CharacterControllerTests, Aim_MoveLeftEvent)
{
    prepareState(*this);
    expectLeftVelocity(DEFAULT_WALK_LEFT_RIGHT_SPEED);
    tiggerAndExpect<MoveLeftEvent>({sut_.animationClipsNames_.armed.walk.moveleft, sut_.animationClipsNames_.aimIdle});
}
TEST_F(CharacterControllerTests, Aim_MoveRightEvent)
{
    prepareState(*this);
    expectLeftVelocity(-DEFAULT_WALK_LEFT_RIGHT_SPEED);
    tiggerAndExpect<MoveRightEvent>(
        {sut_.animationClipsNames_.armed.walk.moveRight, sut_.animationClipsNames_.aimIdle});
}
