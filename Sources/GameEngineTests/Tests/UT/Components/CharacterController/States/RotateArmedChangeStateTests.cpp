#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    test.expectState<DisarmedIdleState>();
    test.expectRotationLeft();
    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.idle});
    test.tiggerAndExpect<RotateLeftEvent, DisarmedRotateState>({test.sut_.animationClipsNames_.disarmed.rotateLeft});
    test.tiggerAndExpect<WeaponStateEvent, RotateArmedChangeState>(
        {test.sut_.animationClipsNames_.armed.rotateLeft, test.sut_.animationClipsNames_.equip});
}
}  // namespace

// TEST_F(CharacterControllerTests, RotateArmedChangeState_WeaponStateEvent)
//{
//     prepareState(*this);
//     tiggerAndExpect<WeaponStateEvent, IdleArmedChangeState>({sut_.animationClipsNames_.disarm},
//                                                             {ADVANCED_TIME_TRANSITION_TIME});
//     Update(ADVANCED_TIME_CLIP_TIME);
//     Update(ADVANCED_TIME_TRANSITION_TIME);
//     expectState<DisarmedIdleState>();
// }
TEST_F(CharacterControllerTests, RotateArmedChangeState_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent, DeathState>({sut_.animationClipsNames_.disarmed.death});
}
TEST_F(CharacterControllerTests, RotateArmedChangeState_EquipEndStateEvent)
{
    prepareState(*this);
    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    tiggerAndExpect<EquipEndStateEvent, ArmedRotateState>({sut_.animationClipsNames_.armed.rotateLeft},
                                                          {ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_CLIP_TIME});
}
TEST_F(CharacterControllerTests, RotateArmedChangeState_DisarmEndStateEvent)
{
    prepareState(*this);
    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    tiggerAndExpect<DisarmEndStateEvent, DisarmedRotateState>({sut_.animationClipsNames_.disarmed.rotateLeft},
                                                              {ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_CLIP_TIME});
}
TEST_F(CharacterControllerTests, RotateArmedChangeState_MoveForwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveForwardEvent, RunAndRotateArmedChangeState>(
        {sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.run.forward});
}
TEST_F(CharacterControllerTests, RotateArmedChangeState_MoveBackwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(-DEFAULT_BACKWARD_RUN_SPEED);
    tiggerAndExpect<MoveBackwardEvent, RunAndRotateArmedChangeState>(
        {sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.run.backward});
}
TEST_F(CharacterControllerTests, RotateArmedChangeState_MoveLeftEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_LEFT, vec3(DEFAULT_WALK_LEFT_RIGHT_SPEED, 0.0, 0));
    tiggerAndExpect<MoveLeftEvent, WalkAndRotateArmedChangeState>(
        {sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.walk.moveleft});
}
TEST_F(CharacterControllerTests, RotateArmedChangeState_MoveRightEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_RIGHT, vec3(DEFAULT_WALK_LEFT_RIGHT_SPEED, 0.0, 0));
    tiggerAndExpect<MoveRightEvent, WalkAndRotateArmedChangeState>(
        {sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.walk.moveRight});
}
TEST_F(CharacterControllerTests, RotateArmedChangeState_RotateLeftEvent)
{
    prepareState(*this);
    expectRotationLeft();
    tiggerAndExpect<RotateLeftEvent, RotateArmedChangeState>(
        {sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.rotateLeft});
}
TEST_F(CharacterControllerTests, RotateArmedChangeState_RotateRightEvent)
{
    prepareState(*this);
    expectRotationRight();
    tiggerAndExpect<RotateRightEvent, RotateArmedChangeState>(
        {sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.rotateRight});
}
TEST_F(CharacterControllerTests, RotateArmedChangeState_RotateTargetEvent)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent, RotateArmedChangeState>(
        RotateTargetEvent{targetRotation.value_},
        {sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.rotateLeft});
}
TEST_F(CharacterControllerTests, RotateArmedChangeState_DrawArrowEvent)
{
    prepareState(*this);
    tiggerAndExpect<DrawArrowEvent, RotateArmedChangeState>(
        {sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.rotateLeft});
    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectRotationLeft(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectState<DrawArrowRotateState>();
}
TEST_F(CharacterControllerTests, RotateArmedChangeState_AimStopEvent)
{
    prepareState(*this);
    tiggerAndExpect<DrawArrowEvent, RotateArmedChangeState>(
        {sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.rotateLeft});
    tiggerAndExpect<AimStopEvent, RotateArmedChangeState>(
        {sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.rotateLeft});
    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectRotationLeft(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectState<ArmedRotateState>();
}
