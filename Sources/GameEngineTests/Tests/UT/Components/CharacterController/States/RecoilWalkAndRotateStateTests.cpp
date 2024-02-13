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
    test.tiggerAndExpect<WalkForwardEvent, RecoilWalkState>(
        {test.sut_.animationClipsNames_.recoilArrow, test.sut_.animationClipsNames_.armed.walk.forward});
    test.tiggerAndExpect<RotateLeftEvent, RecoilWalkAndRotateState>(
        {test.sut_.animationClipsNames_.recoilArrow, test.sut_.animationClipsNames_.armed.walk.forward});
}
}  // namespace

TEST_F(CharacterControllerTests, RecoilWalkAndRotate_RotateLeft)
{
    prepareState(*this);
    tiggerAndExpect<RotateLeftEvent, RecoilWalkAndRotateState>(
        {sut_.animationClipsNames_.recoilArrow, sut_.animationClipsNames_.armed.walk.forward});
}
TEST_F(CharacterControllerTests, RecoilWalkAndRotate_RotateRight)
{
    prepareState(*this);
    tiggerAndExpect<RotateRightEvent, RecoilWalkAndRotateState>(
        {sut_.animationClipsNames_.recoilArrow, sut_.animationClipsNames_.armed.walk.forward});
}
TEST_F(CharacterControllerTests, RecoilWalkAndRotate_RotateTargetEvent)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent, RecoilWalkAndRotateState>(RotateTargetEvent{ targetRotation.value_ },
        {sut_.animationClipsNames_.recoilArrow, sut_.animationClipsNames_.armed.walk.forward});
}
TEST_F(CharacterControllerTests, RecoilWalkAndRotate_WeaponStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<WeaponStateEvent, WalkAndRotateArmedChangeState>(
        {sut_.animationClipsNames_.disarm, sut_.animationClipsNames_.disarmed.walk.forward});

    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectRotationLeft(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectState<DisarmedWalkAndRotateState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.walk.forward});
}
TEST_F(CharacterControllerTests, RecoilWalkAndRotate_ReloadArrowEvent)
{
    prepareState(*this);
    tiggerAndExpect<ReloadArrowEvent, DrawArrowWalkAndRotateState>(
        {sut_.animationClipsNames_.drawArrow, sut_.animationClipsNames_.armed.walk.forward});
}
TEST_F(CharacterControllerTests, RecoilWalkAndRotate_AimStopEvent)
{
    prepareState(*this);
    tiggerAndExpect<AimStopEvent, ArmedWalkAndRotateState>({sut_.animationClipsNames_.armed.walk.forward});
}
TEST_F(CharacterControllerTests, RecoilWalkAndRotate_WalkForwardEvent)
{
    prepareState(*this);
    tiggerAndExpect<WalkForwardEvent, RecoilWalkAndRotateState>(
        {sut_.animationClipsNames_.armed.walk.forward, sut_.animationClipsNames_.recoilArrow});
}
TEST_F(CharacterControllerTests, RecoilWalkAndRotate_WalkBackwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(-DEFAULT_BACKWARD_WALK_SPEED);
    tiggerAndExpect<WalkBackwardEvent, RecoilWalkAndRotateState>(
        {sut_.animationClipsNames_.armed.walk.backward, sut_.animationClipsNames_.recoilArrow});
}
TEST_F(CharacterControllerTests, RecoilWalkAndRotate_WalkChangeStateEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent, RecoilWalkAndRotateState>(
        {sut_.animationClipsNames_.armed.walk.forward, sut_.animationClipsNames_.recoilArrow});
}
TEST_F(CharacterControllerTests, RecoilWalkAndRotate_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent, DeathState>({sut_.animationClipsNames_.armed.death});
}
TEST_F(CharacterControllerTests, RecoilWalkAndRotate_SprintStateChangeEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent, ArmedSprintAndRotateState>({sut_.animationClipsNames_.armed.sprint});
}
TEST_F(CharacterControllerTests, RecoilWalkAndRotate_RunForwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<RunForwardEvent, RecoilWalkAndRotateState>(
        {sut_.animationClipsNames_.armed.walk.forward, sut_.animationClipsNames_.recoilArrow});
}
TEST_F(CharacterControllerTests, RecoilWalkAndRotate_RunBackwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(-DEFAULT_BACKWARD_WALK_SPEED);
    tiggerAndExpect<RunBackwardEvent, RecoilWalkAndRotateState>(
        {sut_.animationClipsNames_.armed.walk.backward, sut_.animationClipsNames_.recoilArrow});
}
TEST_F(CharacterControllerTests, RecoilWalkAndRotate_EndRotationEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<EndRotationEvent, RecoilWalkState>(
        {sut_.animationClipsNames_.armed.walk.forward, sut_.animationClipsNames_.recoilArrow});
}
TEST_F(CharacterControllerTests, RecoilWalkAndRotate_EndForwardMoveEvent)
{
    prepareState(*this);
    tiggerAndExpect<EndForwardMoveEvent, RecoilRotateState>(
        {sut_.animationClipsNames_.armed.rotateLeft, sut_.animationClipsNames_.recoilArrow});
}
TEST_F(CharacterControllerTests, RecoilWalkAndRotate_EndBackwardMoveEvent)
{
    prepareState(*this);
    tiggerAndExpect<EndForwardMoveEvent, RecoilRotateState>(
        {sut_.animationClipsNames_.armed.rotateLeft, sut_.animationClipsNames_.recoilArrow},
        {ADVANCED_TIME_TRANSITION_TIME / 4.f});
    expectForwardVelocity(-DEFAULT_BACKWARD_WALK_SPEED);
    tiggerAndExpect<WalkBackwardEvent, RecoilWalkAndRotateState>(
        {sut_.animationClipsNames_.armed.walk.backward, sut_.animationClipsNames_.recoilArrow});
    tiggerAndExpect<EndBackwardMoveEvent, RecoilRotateState>(
        {sut_.animationClipsNames_.armed.rotateLeft, sut_.animationClipsNames_.recoilArrow},
        {ADVANCED_TIME_TRANSITION_TIME / 4.f});
}
