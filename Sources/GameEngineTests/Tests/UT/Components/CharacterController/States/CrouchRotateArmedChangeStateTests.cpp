#include "../CharacterControllerTests.h"
#include "Components/Controllers/CharacterController/CharacterControllerEvents.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    test.expectRotationLeft();
    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.posture.stand.idle});
    test.tiggerAndExpect<RotateLeftEvent>({test.sut_.animationClipsNames_.disarmed.posture.stand.rotate.left});
    test.tiggerAndExpect<WeaponStateEvent>(
        {test.sut_.animationClipsNames_.armed.posture.stand.rotate.left, test.sut_.animationClipsNames_.equip});
    test.tiggerAndExpect<CrouchChangeStateEvent>(
        {test.sut_.animationClipsNames_.armed.posture.crouched.rotate.left, test.sut_.animationClipsNames_.equip});
}
}  // namespace

// TEST_F(CharacterControllerTests, CrouchRotateArmedChangeState_WeaponStateEvent)
//{
//     prepareState(*this);
//     tiggerAndExpect<WeaponStateEvent, IdleArmedChangeState>({sut_.animationClipsNames_.disarm},
//                                                             {ADVANCED_TIME_TRANSITION_TIME});
//     Update(ADVANCED_TIME_CLIP_TIME);
//     Update(ADVANCED_TIME_TRANSITION_TIME);
//     expectState<DisarmedIdleState>();
// }
TEST_F(CharacterControllerTests, CrouchRotateArmedChangeState_CrouchChangeStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<CrouchChangeStateEvent>(
        {sut_.animationClipsNames_.armed.posture.stand.rotate.left, sut_.animationClipsNames_.equip});
}
TEST_F(CharacterControllerTests, CrouchRotateArmedChangeState_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent>({sut_.animationClipsNames_.disarmed.posture.stand.death});
}
TEST_F(CharacterControllerTests, CrouchRotateArmedChangeState_EquipEndStateEvent)
{
    prepareState(*this);
    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    tiggerAndExpect<EquipEndStateEvent>({sut_.animationClipsNames_.armed.posture.stand.rotate.left},
                                        {ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_CLIP_TIME});
}
TEST_F(CharacterControllerTests, CrouchRotateArmedChangeState_DisarmEndStateEvent)
{
    prepareState(*this);
    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    tiggerAndExpect<DisarmEndStateEvent>({sut_.animationClipsNames_.disarmed.posture.stand.rotate.left},
                                         {ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_CLIP_TIME});
}
TEST_F(CharacterControllerTests, CrouchRotateArmedChangeState_MoveForwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveForwardEvent>({sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.movement.run.forward});
}
TEST_F(CharacterControllerTests, CrouchRotateArmedChangeState_MoveBackwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(-DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveBackwardEvent>({sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.movement.run.forward});
    expectRotationLeft(DEFAULT_MOVING_CHANGE_DIR_SPEED);
    expectRootboneRotation(VECTOR_BACKWARD);
}
TEST_F(CharacterControllerTests, CrouchRotateArmedChangeState_MoveLeftEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_LEFT, vec3(DEFAULT_RUN_SPEED, 0.0, 0.0));
    tiggerAndExpect<MoveLeftEvent>({sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.movement.run.forward});
    expectRotationLeft(DEFAULT_MOVING_CHANGE_DIR_SPEED);
    expectRootboneRotation(VECTOR_LEFT);
}
TEST_F(CharacterControllerTests, CrouchRotateArmedChangeState_MoveRightEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_RIGHT, vec3(DEFAULT_RUN_SPEED, 0.0, 0.0));
    tiggerAndExpect<MoveRightEvent>({sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.movement.run.forward});
    expectRotationLeft(DEFAULT_MOVING_CHANGE_DIR_SPEED);
    expectRootboneRotation(VECTOR_RIGHT);
}
TEST_F(CharacterControllerTests, CrouchRotateArmedChangeState_RotateLeftEvent)
{
    prepareState(*this);
    expectRotationLeft();
    tiggerAndExpect<RotateLeftEvent>(
        {sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.posture.stand.rotate.left});
}
TEST_F(CharacterControllerTests, CrouchRotateArmedChangeState_RotateRightEvent)
{
    prepareState(*this);
    expectRotationRight();
    tiggerAndExpect<RotateRightEvent>(
        {sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.posture.stand.rotate.right});
}
TEST_F(CharacterControllerTests, CrouchRotateArmedChangeState_RotateTargetEvent)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent>(
        RotateTargetEvent{targetRotation.value_},
        {sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.posture.stand.rotate.left});
}
TEST_F(CharacterControllerTests, CrouchRotateArmedChangeState_DrawArrowEvent)
{
    prepareState(*this);
    tiggerAndExpect<DrawArrowEvent>({sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.posture.stand.rotate.left});
    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectRotationLeft(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.aim.draw, sut_.animationClipsNames_.armed.posture.stand.rotate.left});
}
TEST_F(CharacterControllerTests, CrouchRotateArmedChangeState_AimStopEvent)
{
    prepareState(*this);
    tiggerAndExpect<DrawArrowEvent>({sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.posture.stand.rotate.left});
    tiggerAndExpect<AimStopEvent>({sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.posture.stand.rotate.left});
    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectRotationLeft(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.posture.stand.rotate.left});
}
