#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    EXPECT_CALL(test.physicsApiMock_, GetVelocity(test.rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(test.physicsApiMock_, GetRotation(test.rigidbodyid)).WillRepeatedly(Return(Rotation().value_));

    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.idle});

    test.expectForwardVelocity(DEFAULT_RUN_SPEED);
    test.tiggerAndExpect<MoveForwardEvent>({test.sut_.animationClipsNames_.disarmed.run.forward});
    test.expectForwardVelocity(DEFAULT_WALK_SPEED);
    test.tiggerAndExpect<CrouchChangeStateEvent>({test.sut_.animationClipsNames_.disarmed.crouch.forward});
}
}  // namespace

// TEST_F(CharacterControllerTests, DisarmedCruchWalkState_DrawArrowEvent)
//{
//     prepareState(*this);
//     tiggerAndExpect<DrawArrowEvent>({sut_.animationClipsNames_.armed.walk.forward, sut_.animationClipsNames_.equip},
//                                     {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME / 2.f});
//     Update(ADVANCED_TIME_CLIP_TIME / 2.f);
//     Update(ADVANCED_TIME_TRANSITION_TIME);
//     expectAnimsToBeSet({sut_.animationClipsNames_.armed.walk.forward, sut_.animationClipsNames_.drawArrow});
// }

// TEST_F(CharacterControllerTests, DisarmedCruchWalkState_AimStopDuringDrawArrowEvent)
//{
//     prepareState(*this);
//     tiggerAndExpect<DrawArrowEvent>({sut_.animationClipsNames_.armed.walk.forward, sut_.animationClipsNames_.equip},
//                                     {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME / 2.f});
//     tiggerAndExpect<AimStopEvent>({sut_.animationClipsNames_.armed.walk.forward},
//                                   {ADVANCED_TIME_CLIP_TIME / 2.f, ADVANCED_TIME_TRANSITION_TIME});

//    // Extra check
//    Update(ADVANCED_TIME_CLIP_TIME);
//    expectAnimsToBeSet({sut_.animationClipsNames_.armed.walk.forward});
//}

TEST_F(CharacterControllerTests, DisarmedCruchWalkState_CrouchChangeStateEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<CrouchChangeStateEvent>({sut_.animationClipsNames_.disarmed.run.forward},
                                            {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME / 2.f});
}

TEST_F(CharacterControllerTests, DisarmedCruchWalkState_MoveForwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<MoveForwardEvent>({sut_.animationClipsNames_.disarmed.crouch.forward});
}

TEST_F(CharacterControllerTests, DisarmedCruchWalkState_MoveLeftEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_FORWARD + VECTOR_LEFT, vec3(DEFAULT_WALK_SPEED, 0.0, DEFAULT_WALK_SPEED));
    tiggerAndExpect<MoveLeftEvent>({sut_.animationClipsNames_.disarmed.crouch.forward}, {0});
    expectRootboneRotation(VECTOR_FORWARD + VECTOR_LEFT);
}

TEST_F(CharacterControllerTests, DisarmedCruchWalkState_MoveRightEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_FORWARD + VECTOR_RIGHT, vec3(DEFAULT_WALK_SPEED, 0.0, DEFAULT_WALK_SPEED));
    tiggerAndExpect<MoveRightEvent>({sut_.animationClipsNames_.disarmed.crouch.forward}, {0});
    expectRootboneRotation(VECTOR_FORWARD + VECTOR_RIGHT);
}

TEST_F(CharacterControllerTests, DisarmedCruchWalkState_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent>({sut_.animationClipsNames_.disarmed.death});
}

TEST_F(CharacterControllerTests, DisarmedCruchWalkState_WalkChangeStateEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<WalkChangeStateEvent>({sut_.animationClipsNames_.disarmed.run.forward});
}

TEST_F(CharacterControllerTests, DisarmedCruchWalkState_WeaponStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<WeaponStateEvent>(
        {sut_.animationClipsNames_.armed.crouch.forward, sut_.animationClipsNames_.equip});
}

TEST_F(CharacterControllerTests, DisarmedCruchWalkState_EndForwardMoveEvent)
{
    prepareState(*this);
    tiggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.disarmed.crouchIdle});
}

TEST_F(CharacterControllerTests, DisarmedCruchWalkState_EndBackwardMoveEvent)
{
    prepareState(*this);
    expectNoMove();
    tiggerAndExpect<MoveBackwardEvent>({sut_.animationClipsNames_.disarmed.crouchIdle},
                                       {DEFAULT_MOVING_CHANGE_DIR_SPEED});
    expectRootboneRotation(VECTOR_FORWARD);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<EndBackwardMoveEvent>({sut_.animationClipsNames_.disarmed.crouch.forward}, {0});
    expectRootboneRotation(VECTOR_FORWARD);
}

TEST_F(CharacterControllerTests, DisarmedCruchWalkState_MoveBackwardEvent)
{
    prepareState(*this);
    expectNoMove();
    tiggerAndExpect<MoveBackwardEvent>({sut_.animationClipsNames_.disarmed.crouchIdle});
}

// TEST_F(CharacterControllerTests, DisarmedCruchWalkState_RotateLeftEvent)
//{
//     prepareState(*this);
//     expectRotationLeft();
//     tiggerAndExpect<RotateLeftEvent>({sut_.animationClipsNames_.disarmed.walk.forward});
// }

// TEST_F(CharacterControllerTests, DisarmedCruchWalkState_RotateRightEvent)
//{
//     prepareState(*this);
//     expectRotationRight();
//     tiggerAndExpect<RotateRightEvent>({sut_.animationClipsNames_.disarmed.walk.forward});
// }

// TEST_F(CharacterControllerTests, DisarmedCruchWalkState_RotateTargetEvent)
//{
//     prepareState(*this);
//     EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

//    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
//    tiggerAndExpect<RotateTargetEvent>(RotateTargetEvent{targetRotation.value_},
//                                       {sut_.animationClipsNames_.disarmed.walk.forward});
//}

TEST_F(CharacterControllerTests, DisarmedCruchWalkState_SprintStateChangeEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent>({sut_.animationClipsNames_.disarmed.sprint});
}
