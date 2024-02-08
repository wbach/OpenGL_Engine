#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    EXPECT_CALL(test.inputManagerMock_, CalcualteMouseMove()).WillRepeatedly(Return(vec2i{0, 0}));
    EXPECT_CALL(test.physicsApiMock_, GetRotation(test.rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(test.physicsApiMock_, SetRotation(test.rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));
    EXPECT_CALL(test.physicsApiMock_, GetVelocity(test.rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    test.expectForwardVelocity(DEFAULT_RUN_SPEED);

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
    test.tiggerAndExpect<RunForwardEvent, RecoilRunState>(
        {test.sut_.animationClipsNames_.recoilArrow, test.sut_.animationClipsNames_.armed.run.forward});
    test.tiggerAndExpect<RotateLeftEvent, RecoilRunAndRotateState>(
        {test.sut_.animationClipsNames_.recoilArrow, test.sut_.animationClipsNames_.armed.run.forward});
}
}  // namespace

TEST_F(CharacterControllerTests, RecoilRunAndRotate_RotateLeft)
{
    prepareState(*this);
    tiggerAndExpect<RotateLeftEvent, RecoilRunAndRotateState>(
        {sut_.animationClipsNames_.recoilArrow, sut_.animationClipsNames_.armed.run.forward});
}
TEST_F(CharacterControllerTests, RecoilRunAndRotate_RotateRight)
{
    prepareState(*this);
    tiggerAndExpect<RotateRightEvent, RecoilRunAndRotateState>(
        {sut_.animationClipsNames_.recoilArrow, sut_.animationClipsNames_.armed.run.forward});
}
TEST_F(CharacterControllerTests, RecoilRunAndRotate_RotateTargetEvent)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent, RecoilRunAndRotateState>(RotateTargetEvent{ targetRotation.value_ },
        {sut_.animationClipsNames_.recoilArrow, sut_.animationClipsNames_.armed.run.forward});
}
TEST_F(CharacterControllerTests, RecoilRunAndRotate_WeaponStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<WeaponStateEvent, RunAndRotateArmedChangeState>(
        {sut_.animationClipsNames_.disarm, sut_.animationClipsNames_.disarmed.run.forward});

    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectRotationLeft(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectState<DisarmedRunAndRotateState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.run.forward});
}
TEST_F(CharacterControllerTests, RecoilRunAndRotate_ReloadArrowEvent)
{
    prepareState(*this);
    tiggerAndExpect<ReloadArrowEvent, DrawArrowRunAndRotateState>(
        {sut_.animationClipsNames_.drawArrow, sut_.animationClipsNames_.armed.run.forward});
}
TEST_F(CharacterControllerTests, RecoilRunAndRotate_AimStopEvent)
{
    prepareState(*this);
    tiggerAndExpect<AimStopEvent, ArmedRunAndRotateState>({sut_.animationClipsNames_.armed.run.forward});
}
TEST_F(CharacterControllerTests, RecoilRunAndRotate_RunForwardEvent)
{
    prepareState(*this);
    tiggerAndExpect<RunForwardEvent, RecoilRunAndRotateState>(
        {sut_.animationClipsNames_.armed.run.forward, sut_.animationClipsNames_.recoilArrow});
}
TEST_F(CharacterControllerTests, RecoilRunAndRotate_RunBackwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(-DEFAULT_BACKWARD_RUN_SPEED);
    tiggerAndExpect<RunBackwardEvent, RecoilRunAndRotateState>(
        {sut_.animationClipsNames_.armed.run.backward, sut_.animationClipsNames_.recoilArrow});
}
TEST_F(CharacterControllerTests, RecoilRunAndRotate_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent, DeathState>({sut_.animationClipsNames_.armed.death});
}
TEST_F(CharacterControllerTests, RecoilRunAndRotate_SprintStateChangeEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent, ArmedSprintAndRotateState>({sut_.animationClipsNames_.armed.sprint});
}
TEST_F(CharacterControllerTests, RecoilRunAndRotate_WalkChangeStateEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent, RecoilWalkAndRotateState>(
        {sut_.animationClipsNames_.armed.walk.forward, sut_.animationClipsNames_.recoilArrow});
}
TEST_F(CharacterControllerTests, RecoilRunAndRotate_WalkForwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkForwardEvent, RecoilWalkAndRotateState>(
        {sut_.animationClipsNames_.armed.walk.forward, sut_.animationClipsNames_.recoilArrow});
}
TEST_F(CharacterControllerTests, RecoilRunAndRotate_WalkBackwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(-DEFAULT_BACKWARD_WALK_SPEED);
    tiggerAndExpect<WalkBackwardEvent, RecoilWalkAndRotateState>(
        {sut_.animationClipsNames_.armed.walk.backward, sut_.animationClipsNames_.recoilArrow});
}
TEST_F(CharacterControllerTests, RecoilRunAndRotate_EndRotationEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<EndRotationEvent, RecoilRunState>(
        {sut_.animationClipsNames_.armed.run.forward, sut_.animationClipsNames_.recoilArrow});
}
TEST_F(CharacterControllerTests, RecoilRunAndRotate_EndForwardMoveEvent)
{
    prepareState(*this);
    tiggerAndExpect<EndForwardMoveEvent, RecoilRotateState>(
        {sut_.animationClipsNames_.armed.rotateLeft, sut_.animationClipsNames_.recoilArrow});
}
TEST_F(CharacterControllerTests, RecoilRunAndRotate_EndBackwardMoveEvent)
{
    EXPECT_CALL(inputManagerMock_, CalcualteMouseMove()).WillRepeatedly(Return(vec2i{0, 0}));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    expectForwardVelocity(-DEFAULT_BACKWARD_RUN_SPEED);

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    tiggerAndExpect<DrawArrowEvent, DrawArrowState>({sut_.animationClipsNames_.drawArrow});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectState<AimState>();
    tiggerAndExpect<AttackEvent, RecoilState>({sut_.animationClipsNames_.recoilArrow});
    tiggerAndExpect<RunBackwardEvent, RecoilRunState>(
        {sut_.animationClipsNames_.recoilArrow, sut_.animationClipsNames_.armed.run.backward});
    tiggerAndExpect<RotateLeftEvent, RecoilRunAndRotateState>(
        {sut_.animationClipsNames_.recoilArrow, sut_.animationClipsNames_.armed.run.backward});
    tiggerAndExpect<EndBackwardMoveEvent, RecoilRotateState>(
        {sut_.animationClipsNames_.armed.rotateLeft, sut_.animationClipsNames_.recoilArrow});
}
