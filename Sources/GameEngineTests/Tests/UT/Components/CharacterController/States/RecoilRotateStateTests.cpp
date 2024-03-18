#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    EXPECT_CALL(test.inputManagerMock_, CalcualteMouseMove()).WillRepeatedly(Return(vec2i{0, 0}));
    EXPECT_CALL(test.physicsApiMock_, SetRotation(test.rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));
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
    test.tiggerAndExpect<AttackEvent, RecoilState>({test.sut_.animationClipsNames_.recoilArrow});
    test.tiggerAndExpect<RotateLeftEvent, RecoilRotateState>(
        {test.sut_.animationClipsNames_.recoilArrow, test.sut_.animationClipsNames_.armed.rotateLeft});
}
}  // namespace

TEST_F(CharacterControllerTests, RecoilRotate_RotateLeft)
{
    prepareState(*this);
    tiggerAndExpect<RotateLeftEvent, RecoilRotateState>(
        {sut_.animationClipsNames_.recoilArrow, sut_.animationClipsNames_.armed.rotateLeft});
}
TEST_F(CharacterControllerTests, RecoilRotate_RotateRight)
{
    prepareState(*this);
    tiggerAndExpect<RotateRightEvent, RecoilRotateState>(
        {sut_.animationClipsNames_.recoilArrow, sut_.animationClipsNames_.armed.rotateRight});
}
TEST_F(CharacterControllerTests, RecoilRotate_RotateTargetEvent)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent, RecoilRotateState>(
        RotateTargetEvent{targetRotation.value_},
        {sut_.animationClipsNames_.recoilArrow, sut_.animationClipsNames_.armed.rotateLeft});
}
TEST_F(CharacterControllerTests, RecoilRotate_WeaponStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<WeaponStateEvent, RotateArmedChangeState>(
        {sut_.animationClipsNames_.disarm, sut_.animationClipsNames_.disarmed.rotateLeft});

    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectRotationLeft(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectState<DisarmedRotateState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.rotateLeft});
}
TEST_F(CharacterControllerTests, RecoilRotate_AttackEvent)
{
    prepareState(*this);
    tiggerAndExpect<AttackEvent, RecoilRotateState>(
        {sut_.animationClipsNames_.armed.rotateLeft, sut_.animationClipsNames_.recoilArrow});
}
TEST_F(CharacterControllerTests, RecoilRotate_EndRotationEvent)
{
    prepareState(*this);
    tiggerAndExpect<EndRotationEvent, RecoilState>({sut_.animationClipsNames_.recoilArrow});
}
TEST_F(CharacterControllerTests, RecoilRotate_AimStopEvent)
{
    prepareState(*this);
    tiggerAndExpect<AimStopEvent, ArmedRotateState>({sut_.animationClipsNames_.armed.rotateLeft});
}
TEST_F(CharacterControllerTests, RecoilRotate_ReloadArrowEvent)
{
    prepareState(*this);
    tiggerAndExpect<ReloadArrowEvent, DrawArrowRotateState>(
        {sut_.animationClipsNames_.armed.rotateLeft, sut_.animationClipsNames_.drawArrow});
}
TEST_F(CharacterControllerTests, RecoilRotate_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent, DeathState>({sut_.animationClipsNames_.armed.death});
}
TEST_F(CharacterControllerTests, RecoilRotate_MoveForwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<MoveForwardEvent, RecoilWalkAndRotateState>(
        {sut_.animationClipsNames_.recoilArrow, sut_.animationClipsNames_.armed.walk.forward});
}
TEST_F(CharacterControllerTests, RecoilRotate_MoveBackwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(-DEFAULT_BACKWARD_WALK_SPEED);
    tiggerAndExpect<MoveBackwardEvent, RecoilWalkAndRotateState>(
        {sut_.animationClipsNames_.recoilArrow, sut_.animationClipsNames_.armed.walk.backward});
}
TEST_F(CharacterControllerTests, RecoilRotate_MoveLeftEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_LEFT, vec3(DEFAULT_WALK_LEFT_RIGHT_SPEED, 0.0, 0));
    tiggerAndExpect<MoveLeftEvent, RecoilWalkAndRotateState>(
        {sut_.animationClipsNames_.recoilArrow, sut_.animationClipsNames_.armed.walk.moveleft});
}
TEST_F(CharacterControllerTests, RecoilRotate_MoveRightEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_RIGHT, vec3(DEFAULT_WALK_LEFT_RIGHT_SPEED, 0.0, 0));
    tiggerAndExpect<MoveRightEvent, RecoilWalkAndRotateState>(
        {sut_.animationClipsNames_.recoilArrow, sut_.animationClipsNames_.armed.walk.moveRight});
}
