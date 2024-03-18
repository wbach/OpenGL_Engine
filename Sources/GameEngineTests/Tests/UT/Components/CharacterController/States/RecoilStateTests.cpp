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
    test.tiggerAndExpect<AttackEvent, RecoilState>({test.sut_.animationClipsNames_.recoilArrow});
}
}  // namespace

TEST_F(CharacterControllerTests, Recoil_RotateLeft)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));
    tiggerAndExpect<RotateLeftEvent, RecoilRotateState>(
        {sut_.animationClipsNames_.recoilArrow, sut_.animationClipsNames_.armed.rotateLeft});
}
TEST_F(CharacterControllerTests, Recoil_RotateRight)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));
    tiggerAndExpect<RotateRightEvent, RecoilRotateState>(
        {sut_.animationClipsNames_.recoilArrow, sut_.animationClipsNames_.armed.rotateRight});
}
TEST_F(CharacterControllerTests, Recoil_RotateTargetEvent)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent, RecoilRotateState>(RotateTargetEvent{ targetRotation.value_ },
        {sut_.animationClipsNames_.recoilArrow, sut_.animationClipsNames_.armed.rotateLeft });
}
TEST_F(CharacterControllerTests, Recoil_WeaponStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<WeaponStateEvent, IdleArmedChangeState>({sut_.animationClipsNames_.disarm});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
}
TEST_F(CharacterControllerTests, Recoil_AttackEvent)
{
    prepareState(*this);
    tiggerAndExpect<AttackEvent, RecoilState>({sut_.animationClipsNames_.recoilArrow});
}
TEST_F(CharacterControllerTests, Recoil_AimStopEvent)
{
    prepareState(*this);
    tiggerAndExpect<AimStopEvent, ArmedIdleState>({sut_.animationClipsNames_.armed.idle});
}
TEST_F(CharacterControllerTests, Recoil_ReloadArrowEvent)
{
    prepareState(*this);
    tiggerAndExpect<ReloadArrowEvent, DrawArrowState>({sut_.animationClipsNames_.drawArrow});
}
TEST_F(CharacterControllerTests, Recoil_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent, DeathState>({sut_.animationClipsNames_.armed.death});
}
TEST_F(CharacterControllerTests, Recoil_MoveForwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<MoveForwardEvent, RecoilWalkState>(
        {sut_.animationClipsNames_.armed.walk.forward, sut_.animationClipsNames_.recoilArrow});
}
TEST_F(CharacterControllerTests, Recoil_RunBackwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(-DEFAULT_BACKWARD_WALK_SPEED);
    tiggerAndExpect<MoveBackwardEvent, RecoilWalkState>(
        {sut_.animationClipsNames_.armed.walk.backward, sut_.animationClipsNames_.recoilArrow});
}
TEST_F(CharacterControllerTests, Recoil_WalkForwardEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_LEFT, vec3(DEFAULT_WALK_LEFT_RIGHT_SPEED, 0.0, 0));
    tiggerAndExpect<MoveLeftEvent, RecoilWalkState>(
        {sut_.animationClipsNames_.armed.walk.moveleft, sut_.animationClipsNames_.recoilArrow});
}
TEST_F(CharacterControllerTests, Recoil_MoveRightEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_RIGHT, vec3(DEFAULT_WALK_LEFT_RIGHT_SPEED, 0.0, 0));
    tiggerAndExpect<MoveRightEvent, RecoilWalkState>(
        {sut_.animationClipsNames_.armed.walk.moveRight, sut_.animationClipsNames_.recoilArrow});
}
