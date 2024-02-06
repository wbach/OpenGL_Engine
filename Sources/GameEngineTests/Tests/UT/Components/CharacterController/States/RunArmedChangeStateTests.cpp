#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    test.expectState<DisarmedIdleState>();
    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.idle});
    test.expectForwardVelocity(DEFAULT_RUN_SPEED);
    test.tiggerAndExpect<RunForwardEvent, DisarmedRunState>({test.sut_.animationClipsNames_.disarmed.run.forward});
    test.tiggerAndExpect<WeaponStateEvent, RunArmedChangeState>(
        {test.sut_.animationClipsNames_.disarmed.run.forward, test.sut_.animationClipsNames_.equip});
}
}  // namespace

// TEST_F(CharacterControllerTests, RunArmedChangeState_WeaponStateEvent)
//{
//     prepareState(*this);
//     tiggerAndExpect<WeaponStateEvent, IdleArmedChangeState>({sut_.animationClipsNames_.disarm},
//                                                             {ADVANCED_TIME_TRANSITION_TIME});
//     Update(ADVANCED_TIME_CLIP_TIME);
//     Update(ADVANCED_TIME_TRANSITION_TIME);
//     expectState<DisarmedIdleState>();
// }
TEST_F(CharacterControllerTests, RunArmedChangeState_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent, DeathState>({sut_.animationClipsNames_.disarmed.death});
}
TEST_F(CharacterControllerTests, RunArmedChangeState_EquipEndStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<EquipEndStateEvent, ArmedRunState>({sut_.animationClipsNames_.armed.run.forward},
                                                       {ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
}
TEST_F(CharacterControllerTests, RunArmedChangeState_DisarmEndStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<DisarmEndStateEvent, DisarmedRunState>({sut_.animationClipsNames_.disarmed.run.forward},
                                                           {ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
}
TEST_F(CharacterControllerTests, RunArmedChangeState_RunForwardEvent)
{
    prepareState(*this);
    tiggerAndExpect<RunForwardEvent, RunArmedChangeState>(
        {sut_.animationClipsNames_.equip, sut_.animationClipsNames_.disarmed.run.forward});
}
TEST_F(CharacterControllerTests, RunArmedChangeState_RunBackwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(-DEFAULT_BACKWARD_RUN_SPEED);
    tiggerAndExpect<RunBackwardEvent, RunArmedChangeState>(
        {sut_.animationClipsNames_.equip, sut_.animationClipsNames_.disarmed.run.backward});
}
TEST_F(CharacterControllerTests, RunArmedChangeState_WalkChangeStateEventt)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent, WalkArmedChangeState>(
        {sut_.animationClipsNames_.equip, sut_.animationClipsNames_.disarmed.walk.forward});
}
TEST_F(CharacterControllerTests, RunArmedChangeState_SprintStartEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<SprintStartEvent, RunArmedChangeState>(
        {sut_.animationClipsNames_.equip, sut_.animationClipsNames_.disarmed.run.forward},
        {ADVANCED_TIME_TRANSITION_TIME});

    Update(ADVANCED_TIME_CLIP_TIME);
    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectState<ArmedSprintState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.sprint});
}
TEST_F(CharacterControllerTests, RunArmedChangeState_RotateLeftEvent)
{
    prepareState(*this);
    expectRotationLeft();
    tiggerAndExpect<RotateLeftEvent, RunAndRotateArmedChangeState>(
        {sut_.animationClipsNames_.equip, sut_.animationClipsNames_.disarmed.run.forward});
}
TEST_F(CharacterControllerTests, RunArmedChangeState_RotateRightEvent)
{
    prepareState(*this);
    expectRotationRight();
    tiggerAndExpect<RotateRightEvent, RunAndRotateArmedChangeState>(
        {sut_.animationClipsNames_.equip, sut_.animationClipsNames_.disarmed.run.forward});
}
TEST_F(CharacterControllerTests, RunArmedChangeState_RotateTargetEvent)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent, RunAndRotateArmedChangeState>(
        RotateTargetEvent{targetRotation.value_},
        {sut_.animationClipsNames_.equip, sut_.animationClipsNames_.disarmed.run.forward});
}
TEST_F(CharacterControllerTests, RunArmedChangeState_DrawArrowEvent)
{
    prepareState(*this);
    tiggerAndExpect<DrawArrowEvent, RunArmedChangeState>(
        {sut_.animationClipsNames_.disarmed.run.forward, sut_.animationClipsNames_.equip});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectState<DrawArrowRunState>();
}
TEST_F(CharacterControllerTests, RunArmedChangeState_AimStopEvent)
{
    prepareState(*this);
    tiggerAndExpect<DrawArrowEvent, RunArmedChangeState>(
        {sut_.animationClipsNames_.disarmed.run.forward, sut_.animationClipsNames_.equip});
    tiggerAndExpect<AimStopEvent, RunArmedChangeState>(
        {sut_.animationClipsNames_.disarmed.run.forward, sut_.animationClipsNames_.equip});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectState<ArmedRunState>();
}
