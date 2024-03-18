#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    test.expectState<DisarmedIdleState>();
    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.idle});
    test.expectForwardVelocity(DEFAULT_RUN_SPEED);
    test.tiggerAndExpect<MoveForwardEvent, DisarmedRunState>({test.sut_.animationClipsNames_.disarmed.run.forward});
    test.expectForwardVelocity(DEFAULT_WALK_SPEED);
    test.tiggerAndExpect<WalkChangeStateEvent, DisarmedWalkState>(
        {test.sut_.animationClipsNames_.disarmed.walk.forward});
    test.tiggerAndExpect<WeaponStateEvent, WalkArmedChangeState>(
        {test.sut_.animationClipsNames_.armed.walk.forward, test.sut_.animationClipsNames_.equip});
}
}  // namespace

// TEST_F(CharacterControllerTests, WalkArmedChangeState_WeaponStateEvent)
//{
//     prepareState(*this);
//     tiggerAndExpect<WeaponStateEvent, IdleArmedChangeState>({sut_.animationClipsNames_.disarm},
//                                                             {ADVANCED_TIME_TRANSITION_TIME});
//     Update(ADVANCED_TIME_CLIP_TIME);
//     Update(ADVANCED_TIME_TRANSITION_TIME);
//     expectState<DisarmedIdleState>();
// }
TEST_F(CharacterControllerTests, WalkArmedChangeState_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent, DeathState>({sut_.animationClipsNames_.disarmed.death});
}
TEST_F(CharacterControllerTests, WalkArmedChangeState_EquipEndStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<EquipEndStateEvent, ArmedWalkState>({sut_.animationClipsNames_.armed.walk.forward},
                                                        {ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
}
TEST_F(CharacterControllerTests, WalkArmedChangeState_DisarmEndStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<DisarmEndStateEvent, DisarmedWalkState>({sut_.animationClipsNames_.disarmed.walk.forward},
                                                            {ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
}
TEST_F(CharacterControllerTests, WalkArmedChangeState_MoveForwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveForwardEvent, RunArmedChangeState>(
        {sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.run.forward});
}
TEST_F(CharacterControllerTests, WalkArmedChangeState_MoveBackwardEvent)
{
    prepareState(*this);
    expectNoMove();
    tiggerAndExpect<MoveBackwardEvent, RunArmedChangeState>({sut_.animationClipsNames_.equip});
}
TEST_F(CharacterControllerTests, WalkArmedChangeState_MoveLeftEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_LEFT, vec3(DEFAULT_WALK_LEFT_RIGHT_SPEED, 0.0, 0));
    tiggerAndExpect<MoveLeftEvent, RunArmedChangeState>(
        {sut_.animationClipsNames_.equip, sut_.animationClipsNames_.disarmed.run.moveleft});
}
TEST_F(CharacterControllerTests, WalkArmedChangeState_MoveRightEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_RIGHT, vec3(DEFAULT_WALK_LEFT_RIGHT_SPEED, 0.0, 0));
    tiggerAndExpect<MoveRightEvent, RunArmedChangeState>(
        {sut_.animationClipsNames_.equip, sut_.animationClipsNames_.disarmed.run.moveRight});
}
TEST_F(CharacterControllerTests, WalkArmedChangeState_WalkChangeStateEventt)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<WalkChangeStateEvent, RunArmedChangeState>(
        {sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.run.forward});
}
TEST_F(CharacterControllerTests, WalkArmedChangeState_SprintStateChangeEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<SprintStateChangeEvent, RunArmedChangeState>(
        {sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.run.forward},
        {ADVANCED_TIME_TRANSITION_TIME});

    Update(ADVANCED_TIME_CLIP_TIME);
    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectState<ArmedSprintState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.sprint});
}
TEST_F(CharacterControllerTests, WalkArmedChangeState_RotateLeftEvent)
{
    prepareState(*this);
    expectRotationLeft();
    tiggerAndExpect<RotateLeftEvent, WalkAndRotateArmedChangeState>(
        {sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.walk.forward});
}
TEST_F(CharacterControllerTests, WalkArmedChangeState_RotateRightEvent)
{
    prepareState(*this);
    expectRotationRight();
    tiggerAndExpect<RotateRightEvent, WalkAndRotateArmedChangeState>(
        {sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.walk.forward});
}
TEST_F(CharacterControllerTests, WalkArmedChangeState_RotateTargetEvent)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent, WalkAndRotateArmedChangeState>(
        RotateTargetEvent{targetRotation.value_},
        {sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.walk.forward});
}
TEST_F(CharacterControllerTests, WalkArmedChangeState_DrawArrowEvent)
{
    prepareState(*this);
    tiggerAndExpect<DrawArrowEvent, WalkArmedChangeState>(
        {sut_.animationClipsNames_.armed.walk.forward, sut_.animationClipsNames_.equip});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectState<DrawArrowWalkState>();
}
TEST_F(CharacterControllerTests, WalkArmedChangeState_AimStopEvent)
{
    prepareState(*this);
    tiggerAndExpect<DrawArrowEvent, WalkArmedChangeState>(
        {sut_.animationClipsNames_.armed.walk.forward, sut_.animationClipsNames_.equip});
    tiggerAndExpect<AimStopEvent, WalkArmedChangeState>(
        {sut_.animationClipsNames_.armed.walk.forward, sut_.animationClipsNames_.equip});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectState<ArmedWalkState>();
}
