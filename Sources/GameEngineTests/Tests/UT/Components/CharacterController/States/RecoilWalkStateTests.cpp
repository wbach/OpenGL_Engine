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
    test.tiggerAndExpect<AttackEvent, RecoilState>({test.sut_.animationClipsNames_.recoilArrow});
    test.tiggerAndExpect<MoveForwardEvent, RecoilWalkState>(
        {test.sut_.animationClipsNames_.recoilArrow, test.sut_.animationClipsNames_.armed.walk.forward});
}
}  // namespace

TEST_F(CharacterControllerTests, RecoilWalk_RotateLeft)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));
    tiggerAndExpect<RotateLeftEvent, RecoilWalkAndRotateState>(
        {sut_.animationClipsNames_.recoilArrow, sut_.animationClipsNames_.armed.walk.forward});
}
TEST_F(CharacterControllerTests, RecoilWalk_RotateRight)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));
    tiggerAndExpect<RotateRightEvent, RecoilWalkAndRotateState>(
        {sut_.animationClipsNames_.recoilArrow, sut_.animationClipsNames_.armed.walk.forward});
}
TEST_F(CharacterControllerTests, RecoilWalk_RotateTargetEvent)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent, RecoilWalkAndRotateState>(
        RotateTargetEvent{targetRotation.value_},
        {sut_.animationClipsNames_.recoilArrow, sut_.animationClipsNames_.armed.walk.forward});
}
TEST_F(CharacterControllerTests, RecoilWalk_WeaponStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<WeaponStateEvent, WalkArmedChangeState>(
        {sut_.animationClipsNames_.disarm, sut_.animationClipsNames_.disarmed.walk.forward});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectState<DisarmedWalkState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.walk.forward});
}
TEST_F(CharacterControllerTests, RecoilWalk_ReloadArrowEvent)
{
    prepareState(*this);
    tiggerAndExpect<ReloadArrowEvent, DrawArrowWalkState>(
        {sut_.animationClipsNames_.drawArrow, sut_.animationClipsNames_.armed.walk.forward});
}
TEST_F(CharacterControllerTests, RecoilWalk_AimStopEvent)
{
    prepareState(*this);
    tiggerAndExpect<AimStopEvent, ArmedWalkState>({sut_.animationClipsNames_.armed.walk.forward});
}
TEST_F(CharacterControllerTests, RecoilWalk_MoveForwardEvent)
{
    prepareState(*this);
    tiggerAndExpect<MoveForwardEvent, RecoilWalkState>(
        {sut_.animationClipsNames_.recoilArrow, sut_.animationClipsNames_.armed.walk.forward});
}
TEST_F(CharacterControllerTests, RecoilWalk_MoveBackwardEvent)
{
    prepareState(*this);
    expectNoMove();
    tiggerAndExpect<MoveBackwardEvent, RecoilWalkState>({sut_.animationClipsNames_.recoilArrow});
}
TEST_F(CharacterControllerTests, RecoilWalk_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent, DeathState>({sut_.animationClipsNames_.armed.death});
}
TEST_F(CharacterControllerTests, RecoilWalk_SprintStateChangeEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent, ArmedSprintState>({sut_.animationClipsNames_.armed.sprint});
}
TEST_F(CharacterControllerTests, RecoilWalk_WalkChangeStateEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent, RecoilWalkState>(
        {sut_.animationClipsNames_.recoilArrow, sut_.animationClipsNames_.armed.walk.forward});
}
TEST_F(CharacterControllerTests, RecoilWalk_MoveLeftEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_LEFT, vec3(DEFAULT_WALK_LEFT_RIGHT_SPEED, 0.0, 0));
    tiggerAndExpect<MoveLeftEvent, RecoilWalkState>(
        {sut_.animationClipsNames_.recoilArrow, sut_.animationClipsNames_.armed.walk.moveleft});
}
TEST_F(CharacterControllerTests, RecoilWalk_RunBackwardEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_RIGHT, vec3(DEFAULT_WALK_LEFT_RIGHT_SPEED, 0.0, 0));
    tiggerAndExpect<EndForwardMoveEvent, RecoilWalkState>(
        {sut_.animationClipsNames_.recoilArrow, sut_.animationClipsNames_.armed.walk.moveRight});
}
TEST_F(CharacterControllerTests, RecoilWalk_EndForwardMoveEvent)
{
    prepareState(*this);
    tiggerAndExpect<EndForwardMoveEvent, RecoilState>({sut_.animationClipsNames_.recoilArrow});
}
TEST_F(CharacterControllerTests, RecoilWalk_EndBackwardMoveEvent)
{
    prepareState(*this);
    tiggerAndExpect<EndForwardMoveEvent, RecoilState>({sut_.animationClipsNames_.recoilArrow});
    expectForwardVelocity(-DEFAULT_BACKWARD_WALK_SPEED);
    tiggerAndExpect<MoveBackwardEvent, RecoilWalkState>(
        {sut_.animationClipsNames_.recoilArrow, sut_.animationClipsNames_.armed.walk.backward});
    tiggerAndExpect<EndBackwardMoveEvent, RecoilState>({sut_.animationClipsNames_.recoilArrow});
}
TEST_F(CharacterControllerTests, RecoilWalk_ReloadArrowEventWhenRunLeftAndRight)
{
    prepareState(*this);
    tiggerAndExpect<EndForwardMoveEvent, RecoilState>({sut_.animationClipsNames_.recoilArrow});
    expectVelocity(VECTOR_LEFT, vec3(DEFAULT_WALK_LEFT_RIGHT_SPEED, 0.f, 0.f));
    tiggerAndExpect<MoveLeftEvent, RecoilWalkState>(
        {sut_.animationClipsNames_.recoilArrow, sut_.animationClipsNames_.armed.walk.moveleft});
    expectNoMove();
    tiggerAndExpect<MoveRightEvent, RecoilWalkState>({sut_.animationClipsNames_.recoilArrow});
    expectNoMove();
    tiggerAndExpect<ReloadArrowEvent, DrawArrowWalkState>({sut_.animationClipsNames_.drawArrow});
}
