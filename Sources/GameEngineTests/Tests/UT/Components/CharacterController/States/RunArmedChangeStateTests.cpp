#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.posture.stand.idle});
    test.expectForwardVelocity(DEFAULT_RUN_SPEED);
    test.tiggerAndExpect<MoveForwardEvent>({test.sut_.animationClipsNames_.disarmed.movement.run.forward});
    test.tiggerAndExpect<WeaponStateEvent>(
        {test.sut_.animationClipsNames_.armed.movement.run.forward, test.sut_.animationClipsNames_.equip});
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
TEST_F(CharacterControllerTests, RunArmedChangeState_CrouchChangeStateEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<CrouchChangeStateEvent>(
        {sut_.animationClipsNames_.armed.movement.crouch.forward, sut_.animationClipsNames_.equip});
}
TEST_F(CharacterControllerTests, RunArmedChangeState_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent>({sut_.animationClipsNames_.disarmed.posture.stand.death});
}
TEST_F(CharacterControllerTests, RunArmedChangeState_EquipEndStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<EquipEndStateEvent>({sut_.animationClipsNames_.armed.movement.run.forward},
                                        {ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
}
TEST_F(CharacterControllerTests, RunArmedChangeState_DisarmEndStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<DisarmEndStateEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward},
                                         {ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
}
TEST_F(CharacterControllerTests, RunArmedChangeState_MoveForwardEvent)
{
    prepareState(*this);
    tiggerAndExpect<MoveForwardEvent>({sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.movement.run.forward});
}
TEST_F(CharacterControllerTests, RunArmedChangeState_MoveBackwardEvent)
{
    prepareState(*this);
    expectNoMove();
    tiggerAndExpect<MoveBackwardEvent>({sut_.animationClipsNames_.equip});
}
TEST_F(CharacterControllerTests, RunArmedChangeState_MoveLeftEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_FORWARD + VECTOR_LEFT, vec3(DEFAULT_RUN_SPEED, 0.0, DEFAULT_RUN_SPEED));
    tiggerAndExpect<MoveLeftEvent>({sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.movement.run.forward});
    expectRootboneRotation(VECTOR_FORWARD + VECTOR_LEFT);
}
TEST_F(CharacterControllerTests, RunArmedChangeState_MoveRightEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_FORWARD + VECTOR_RIGHT, vec3(DEFAULT_RUN_SPEED, 0.0, DEFAULT_RUN_SPEED));
    tiggerAndExpect<MoveRightEvent>({sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.movement.run.forward});
    expectRootboneRotation(VECTOR_FORWARD + VECTOR_RIGHT);
}
TEST_F(CharacterControllerTests, RunArmedChangeState_WalkChangeStateEventt)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent>(
        {sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.movement.walk.forward});
}
TEST_F(CharacterControllerTests, RunArmedChangeState_SprintStateChangeEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<SprintStateChangeEvent>(
        {sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.movement.run.forward}, {ADVANCED_TIME_TRANSITION_TIME});

    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.sprint});
}
TEST_F(CharacterControllerTests, RunArmedChangeState_RotateLeftEvent)
{
    prepareState(*this);
    expectRotationLeft();
    tiggerAndExpect<RotateLeftEvent>({sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.movement.run.forward});
}
TEST_F(CharacterControllerTests, RunArmedChangeState_RotateRightEvent)
{
    prepareState(*this);
    expectRotationRight();
    tiggerAndExpect<RotateRightEvent>({sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.movement.run.forward});
}
TEST_F(CharacterControllerTests, RunArmedChangeState_RotateTargetEvent)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent>(RotateTargetEvent{targetRotation.value_},
                                       {sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.movement.run.forward});
}
TEST_F(CharacterControllerTests, RunArmedChangeState_DrawArrowEvent)
{
    prepareState(*this);
    tiggerAndExpect<DrawArrowEvent>({sut_.animationClipsNames_.armed.movement.run.forward, sut_.animationClipsNames_.equip});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.movement.walk.forward, sut_.animationClipsNames_.aim.draw});
}
TEST_F(CharacterControllerTests, RunArmedChangeState_AimStopEvent)
{
    prepareState(*this);
    tiggerAndExpect<DrawArrowEvent>({sut_.animationClipsNames_.armed.movement.run.forward, sut_.animationClipsNames_.equip});
    tiggerAndExpect<AimStopEvent>({sut_.animationClipsNames_.armed.movement.run.forward, sut_.animationClipsNames_.equip});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.movement.run.forward});
}
