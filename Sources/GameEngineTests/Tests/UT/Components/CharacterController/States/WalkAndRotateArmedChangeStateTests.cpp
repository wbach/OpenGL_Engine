#include "../CharacterControllerTests.h"

namespace
{
    void prepareState(CharacterControllerTests& test)
    {
        test.expectState<DisarmedIdleState>();
        test.expectAnimsToBeSet({ test.sut_.animationClipsNames_.disarmed.idle });
        test.expectForwardVelocity(DEFAULT_WALK_SPEED);
        test.expectRotationRight();
        test.tiggerAndExpect<WalkForwardEvent, DisarmedWalkState>({ test.sut_.animationClipsNames_.disarmed.walk.forward });
        test.tiggerAndExpect<RotateRightEvent, DisarmedWalkAndRotateState>(
            { test.sut_.animationClipsNames_.disarmed.walk.forward });
        test.tiggerAndExpect<WeaponStateEvent, WalkAndRotateArmedChangeState>(
            { test.sut_.animationClipsNames_.armed.walk.forward, test.sut_.animationClipsNames_.equip });
    }
}  // namespace

// TEST_F(CharacterControllerTests, WalkAndRotateArmedChangeState_WeaponStateEvent)
//{
//     prepareState(*this);
//     tiggerAndExpect<WeaponStateEvent, IdleArmedChangeState>({sut_.animationClipsNames_.disarm},
//                                                             {ADVANCED_TIME_TRANSITION_TIME});
//     Update(ADVANCED_TIME_CLIP_TIME);
//     Update(ADVANCED_TIME_TRANSITION_TIME);
//     expectState<DisarmedIdleState>();
// }
TEST_F(CharacterControllerTests, WalkAndRotateArmedChangeState_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent, DeathState>({ sut_.animationClipsNames_.disarmed.death });
}
TEST_F(CharacterControllerTests, WalkAndRotateArmedChangeState_EquipEndStateEvent)
{
    prepareState(*this);
    expectRotationRight(ADVANCED_TIME_CLIP_TIME);
    tiggerAndExpect<EquipEndStateEvent, ArmedWalkAndRotateState>({ sut_.animationClipsNames_.armed.walk.forward },
        { ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_CLIP_TIME });
}
TEST_F(CharacterControllerTests, WalkAndRotateArmedChangeState_DisarmEndStateEvent)
{
    prepareState(*this);
    expectRotationRight(ADVANCED_TIME_CLIP_TIME);
    tiggerAndExpect<DisarmEndStateEvent, DisarmedWalkAndRotateState>({ sut_.animationClipsNames_.disarmed.walk.forward },
        { ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_CLIP_TIME });
}
TEST_F(CharacterControllerTests, WalkAndRotateArmedChangeState_RunForwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<RunForwardEvent, RunAndRotateArmedChangeState>(
        { sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.run.forward });
}
TEST_F(CharacterControllerTests, WalkAndRotateArmedChangeState_RunBackwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(-DEFAULT_BACKWARD_RUN_SPEED);
    tiggerAndExpect<RunBackwardEvent, RunAndRotateArmedChangeState>(
        { sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.run.backward });
}
TEST_F(CharacterControllerTests, WalkAndRotateArmedChangeState_WalkChangeStateEventt)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<WalkChangeStateEvent, RunAndRotateArmedChangeState>(
        { sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.run.forward });
}
TEST_F(CharacterControllerTests, WalkAndRotateArmedChangeState_SprintStateChangeEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<SprintStateChangeEvent, RunAndRotateArmedChangeState>(
        { sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.run.forward },
        { ADVANCED_TIME_TRANSITION_TIME });

    expectRotationRight(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    expectRotationRight();
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectState<ArmedSprintAndRotateState>();
    expectAnimsToBeSet({ sut_.animationClipsNames_.armed.sprint });
}
TEST_F(CharacterControllerTests, WalkAndRotateArmedChangeState_RotateLeftEvent)
{
    prepareState(*this);
    expectRotationLeft();
    tiggerAndExpect<RotateLeftEvent, WalkAndRotateArmedChangeState>(
        { sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.walk.forward });
}
TEST_F(CharacterControllerTests, WalkAndRotateArmedChangeState_RotateRightEvent)
{
    prepareState(*this);
    expectRotationRight();
    tiggerAndExpect<RotateRightEvent, WalkAndRotateArmedChangeState>(
        { sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.walk.forward });
}
TEST_F(CharacterControllerTests, WalkAndRotateArmedChangeState_RotateTargetEvent)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent, WalkAndRotateArmedChangeState>(
        RotateTargetEvent{ targetRotation.value_ },
        { sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.walk.forward });
}
TEST_F(CharacterControllerTests, WalkAndRotateArmedChangeState_DrawArrowEvent)
{
    prepareState(*this);
    tiggerAndExpect<DrawArrowEvent, WalkAndRotateArmedChangeState>(
        { sut_.animationClipsNames_.armed.walk.forward, sut_.animationClipsNames_.equip });
    expectRotationRight(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectRotationRight();
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectState<DrawArrowWalkAndRotateState>();
}
TEST_F(CharacterControllerTests, WalkAndRotateArmedChangeState_AimStopEvent)
{
    prepareState(*this);
    tiggerAndExpect<DrawArrowEvent, WalkAndRotateArmedChangeState>(
        { sut_.animationClipsNames_.armed.walk.forward, sut_.animationClipsNames_.equip });
    tiggerAndExpect<AimStopEvent, WalkAndRotateArmedChangeState>(
        { sut_.animationClipsNames_.armed.walk.forward, sut_.animationClipsNames_.equip });
    expectRotationRight(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectRotationRight();
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectState<ArmedWalkAndRotateState>();
}
