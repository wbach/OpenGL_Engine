#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    test.expectState<DisarmedIdleState>();
    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.idle});
    test.expectForwardVelocity(DEFAULT_RUN_SPEED);
    test.expectRotationRight();
    test.tiggerAndExpect<RunForwardEvent, DisarmedRunState>({test.sut_.animationClipsNames_.disarmed.run.forward});
    test.tiggerAndExpect<RotateRightEvent, DisarmedRunAndRotateState>(
        {test.sut_.animationClipsNames_.disarmed.run.forward});
    test.tiggerAndExpect<WeaponStateEvent, RunAndRotateArmedChangeState>(
        {test.sut_.animationClipsNames_.armed.run.forward, test.sut_.animationClipsNames_.equip});
}
}  // namespace

// TEST_F(CharacterControllerTests, RunAndRotateArmedChangeState_WeaponStateEvent)
//{
//     prepareState(*this);
//     tiggerAndExpect<WeaponStateEvent, IdleArmedChangeState>({sut_.animationClipsNames_.disarm},
//                                                             {ADVANCED_TIME_TRANSITION_TIME});
//     Update(ADVANCED_TIME_CLIP_TIME);
//     Update(ADVANCED_TIME_TRANSITION_TIME);
//     expectState<DisarmedIdleState>();
// }
TEST_F(CharacterControllerTests, RunAndRotateArmedChangeState_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent, DeathState>({sut_.animationClipsNames_.disarmed.death});
}
TEST_F(CharacterControllerTests, RunAndRotateArmedChangeState_EquipEndStateEvent)
{
    prepareState(*this);
    expectRotationRight(ADVANCED_TIME_CLIP_TIME);
    tiggerAndExpect<EquipEndStateEvent, ArmedRunAndRotateState>({sut_.animationClipsNames_.armed.run.forward},
                                                                {ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_CLIP_TIME});
}
TEST_F(CharacterControllerTests, RunAndRotateArmedChangeState_DisarmEndStateEvent)
{
    prepareState(*this);
    expectRotationRight(ADVANCED_TIME_CLIP_TIME);
    tiggerAndExpect<DisarmEndStateEvent, DisarmedRunAndRotateState>({sut_.animationClipsNames_.disarmed.run.forward},
                                                                    {ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_CLIP_TIME});
}
TEST_F(CharacterControllerTests, RunAndRotateArmedChangeState_RunForwardEvent)
{
    prepareState(*this);
    tiggerAndExpect<RunForwardEvent, RunAndRotateArmedChangeState>(
        {sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.run.forward});
}
TEST_F(CharacterControllerTests, RunAndRotateArmedChangeState_RunBackwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(-DEFAULT_BACKWARD_RUN_SPEED);
    tiggerAndExpect<RunBackwardEvent, RunAndRotateArmedChangeState>(
        {sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.run.backward});
}
TEST_F(CharacterControllerTests, RunAndRotateArmedChangeState_WalkChangeStateEventt)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent, WalkAndRotateArmedChangeState>(
        {sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.walk.forward});
}
TEST_F(CharacterControllerTests, RunAndRotateArmedChangeState_SprintStateChangeEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<SprintStateChangeEvent, RunAndRotateArmedChangeState>(
        {sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.run.forward},
        {ADVANCED_TIME_TRANSITION_TIME});

    expectRotationRight(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    expectRotationRight();
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectState<ArmedSprintAndRotateState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.sprint});
}
TEST_F(CharacterControllerTests, RunAndRotateArmedChangeState_RotateLeftEvent)
{
    prepareState(*this);
    expectRotationLeft();
    tiggerAndExpect<RotateLeftEvent, RunAndRotateArmedChangeState>(
        {sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.run.forward});
}
TEST_F(CharacterControllerTests, RunAndRotateArmedChangeState_RotateRightEvent)
{
    prepareState(*this);
    expectRotationRight();
    tiggerAndExpect<RotateRightEvent, RunAndRotateArmedChangeState>(
        {sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.run.forward});
}
TEST_F(CharacterControllerTests, RunAndRotateArmedChangeState_RotateTargetEvent)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent, RunAndRotateArmedChangeState>(
        RotateTargetEvent{targetRotation.value_},
        {sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.run.forward});
}
TEST_F(CharacterControllerTests, RunAndRotateArmedChangeState_DrawArrowEvent)
{
    prepareState(*this);
    tiggerAndExpect<DrawArrowEvent, RunAndRotateArmedChangeState>(
        {sut_.animationClipsNames_.armed.run.forward, sut_.animationClipsNames_.equip});
    expectRotationRight(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectRotationRight();
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectState<DrawArrowRunAndRotateState>();
}
TEST_F(CharacterControllerTests, RunAndRotateArmedChangeState_AimStopEvent)
{
    prepareState(*this);
    tiggerAndExpect<DrawArrowEvent, RunAndRotateArmedChangeState>(
        {sut_.animationClipsNames_.armed.run.forward, sut_.animationClipsNames_.equip});
    tiggerAndExpect<AimStopEvent, RunAndRotateArmedChangeState>(
        {sut_.animationClipsNames_.armed.run.forward, sut_.animationClipsNames_.equip});
    expectRotationRight(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectRotationRight();
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectState<ArmedRunAndRotateState>();
}
