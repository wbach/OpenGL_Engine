#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    test.expectState<DisarmedIdleState>();
    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.idle});
    test.tiggerAndExpect<WeaponStateEvent, IdleArmedChangeState>({test.sut_.animationClipsNames_.equip});
}
}  // namespace

// TEST_F(CharacterControllerTests, IdleArmedChangeState_WeaponStateEvent)
//{
//     prepareState(*this);
//     tiggerAndExpect<WeaponStateEvent, IdleArmedChangeState>({sut_.animationClipsNames_.disarm},
//                                                             {ADVANCED_TIME_TRANSITION_TIME});
//     Update(ADVANCED_TIME_CLIP_TIME);
//     Update(ADVANCED_TIME_TRANSITION_TIME);
//     expectState<DisarmedIdleState>();
// }
TEST_F(CharacterControllerTests, IdleArmedChangeState_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent, DeathState>({sut_.animationClipsNames_.disarmed.death});
}
TEST_F(CharacterControllerTests, IdleArmedChangeState_EquipEndStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<EquipEndStateEvent, ArmedIdleState>({sut_.animationClipsNames_.armed.idle},
                                                        {ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
}
TEST_F(CharacterControllerTests, IdleArmedChangeState_DisarmEndStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<DisarmEndStateEvent, DisarmedIdleState>({sut_.animationClipsNames_.disarmed.idle},
                                                            {ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
}
TEST_F(CharacterControllerTests, IdleArmedChangeState_MoveForwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveForwardEvent, RunArmedChangeState>(
        {sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.run.forward});
}
TEST_F(CharacterControllerTests, IdleArmedChangeState_MoveBackwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(-DEFAULT_BACKWARD_RUN_SPEED);
    tiggerAndExpect<MoveBackwardEvent, RunArmedChangeState>(
        {sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.run.backward});
}
TEST_F(CharacterControllerTests, IdleArmedChangeState_MoveLeftEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_LEFT, vec3(DEFAULT_WALK_LEFT_RIGHT_SPEED, 0.0, 0));
    tiggerAndExpect<MoveLeftEvent, WalkArmedChangeState>(
        {sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.walk.moveleft});
}
TEST_F(CharacterControllerTests, IdleArmedChangeState_MoveRightEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_RIGHT, vec3(DEFAULT_WALK_LEFT_RIGHT_SPEED, 0.0, 0));
    tiggerAndExpect<MoveRightEvent, WalkArmedChangeState>(
        {sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.walk.moveRight});
}
TEST_F(CharacterControllerTests, IdleArmedChangeState_RotateLeftEvent)
{
    prepareState(*this);
    expectRotationLeft();
    tiggerAndExpect<RotateLeftEvent, RotateArmedChangeState>(
        {sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.rotateLeft});
}
TEST_F(CharacterControllerTests, IdleArmedChangeState_RotateRightEvent)
{
    prepareState(*this);
    expectRotationRight();
    tiggerAndExpect<RotateRightEvent, RotateArmedChangeState>(
        {sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.rotateRight});
}
TEST_F(CharacterControllerTests, IdleArmedChangeState_RotateTargetEvent)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent, RotateArmedChangeState>(
        RotateTargetEvent{targetRotation.value_},
        {sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.rotateLeft});
}
TEST_F(CharacterControllerTests, IdleArmedChangeState_DrawArrowEvent)
{
    prepareState(*this);
    tiggerAndExpect<DrawArrowEvent, IdleArmedChangeState>({sut_.animationClipsNames_.equip});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectState<DrawArrowState>();
}
TEST_F(CharacterControllerTests, IdleArmedChangeState_AimStopEvent)
{
    prepareState(*this);
    tiggerAndExpect<DrawArrowEvent, IdleArmedChangeState>({sut_.animationClipsNames_.equip});
    tiggerAndExpect<AimStopEvent, IdleArmedChangeState>({sut_.animationClipsNames_.equip});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectState<ArmedIdleState>();
}
