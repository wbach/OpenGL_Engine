#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    for (auto& state : test.sut_.animationClipsNames_.disarmed.attack)
    {
        state.stateType = GameEngine::Components::PlayStateType::run;
    }

    EXPECT_CALL(test.physicsApiMock_, GetVelocity(test.rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(test.physicsApiMock_, GetRotation(test.rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    test.expectForwardVelocity(DEFAULT_RUN_SPEED);

    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.idle});
    test.tiggerAndExpect<MoveForwardEvent>({test.sut_.animationClipsNames_.disarmed.run.forward});
    test.expectForwardVelocity(DEFAULT_WALK_SPEED);
    test.tiggerAndExpect<WalkChangeStateEvent>({test.sut_.animationClipsNames_.disarmed.walk.forward});
    test.tiggerAndExpect<AttackEvent>({test.sut_.animationClipsNames_.disarmed.attack.front().name,
                                       test.sut_.animationClipsNames_.disarmed.walk.forward});
}
}  // namespace

TEST_F(CharacterControllerTests, DisarmedAttackAndWalkState_DrawArrowEvent)
{
    prepareState(*this);
    // queue DrawArrowEvent
    tiggerAndExpect<DrawArrowEvent>(
        {sut_.animationClipsNames_.disarmed.attack.front().name, sut_.animationClipsNames_.disarmed.walk.forward});

    // wait until attack finish
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.walk.forward, sut_.animationClipsNames_.equip});

    Update(ADVANCED_TIME_CLIP_TIME);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.walk.forward, sut_.animationClipsNames_.drawArrow});
}

TEST_F(CharacterControllerTests, DisarmedAttackAndWalkState_AimStopDuringDrawArrowEvent)
{
    prepareState(*this);
    // queue DrawArrowEvent
    tiggerAndExpect<DrawArrowEvent>(
        {sut_.animationClipsNames_.disarmed.attack.front().name, sut_.animationClipsNames_.disarmed.walk.forward});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    tiggerAndExpect<AimStopEvent>(
        {sut_.animationClipsNames_.disarmed.attack.front().name, sut_.animationClipsNames_.disarmed.walk.forward});

    // Extra check
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.walk.forward});
}

TEST_F(CharacterControllerTests, DisarmedAttackAndWalkState_MoveForwardEvent)
{
    prepareState(*this);
    const auto& attackClipName = sut_.animationClipsNames_.disarmed.attack.front().name;

    tiggerAndExpect<EndForwardMoveEvent>({attackClipName});

    expectForwardVelocity(-DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveBackwardEvent>({attackClipName, sut_.animationClipsNames_.disarmed.run.forward}, {0});
    expectRootboneRotation(VECTOR_BACKWARD);

    expectForwardVelocity(-DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent>({attackClipName, sut_.animationClipsNames_.disarmed.walk.forward}, {0});

    expectNoMove();
    tiggerAndExpect<MoveForwardEvent>({attackClipName});
}

TEST_F(CharacterControllerTests, DisarmedAttackAndWalkState_MoveBackwardEvent)
{
    const auto& attackClipName = sut_.animationClipsNames_.disarmed.attack.front().name;
    prepareState(*this);
    expectNoMove();
    tiggerAndExpect<MoveBackwardEvent>({attackClipName});
}

TEST_F(CharacterControllerTests, DisarmedAttackAndWalkState_MoveLeftEvent_EndMoveLeftEvent)
{
    prepareState(*this);
    const auto& attackClipName = sut_.animationClipsNames_.disarmed.attack.front().name;

    expectVelocity(VECTOR_FORWARD + VECTOR_LEFT, vec3(DEFAULT_WALK_SPEED, 0.0, DEFAULT_WALK_SPEED));
    tiggerAndExpect<MoveLeftEvent>({attackClipName, sut_.animationClipsNames_.disarmed.walk.forward}, {0});
    expectRootboneRotation(VECTOR_FORWARD + VECTOR_LEFT);
    expectVelocity(VECTOR_LEFT, vec3(DEFAULT_WALK_SPEED, 0.0, 0.0));
    tiggerAndExpect<EndForwardMoveEvent>({attackClipName, sut_.animationClipsNames_.disarmed.walk.forward}, {0});
    expectRootboneRotation(VECTOR_LEFT);
    tiggerAndExpect<EndMoveLeftEvent>({attackClipName}, {0});
}

TEST_F(CharacterControllerTests, DisarmedAttackAndWalkState_MoveRightEvent_EndMoveRightEvent)
{
    prepareState(*this);
    const auto& attackClipName = sut_.animationClipsNames_.disarmed.attack.front().name;

    expectVelocity(VECTOR_FORWARD + VECTOR_RIGHT, vec3(DEFAULT_WALK_SPEED, 0.0, DEFAULT_WALK_SPEED));
    tiggerAndExpect<MoveRightEvent>({attackClipName, sut_.animationClipsNames_.disarmed.walk.forward}, {0});
    expectRootboneRotation(VECTOR_FORWARD + VECTOR_RIGHT);
    expectVelocity(VECTOR_RIGHT, vec3(DEFAULT_WALK_SPEED, 0.0, 0.0));
    tiggerAndExpect<EndForwardMoveEvent>({attackClipName, sut_.animationClipsNames_.disarmed.walk.forward}, {0});
    expectRootboneRotation(VECTOR_RIGHT);
    tiggerAndExpect<EndMoveRightEvent>({attackClipName}, {0});
}

TEST_F(CharacterControllerTests, DisarmedAttackAndWalkState_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent>({sut_.animationClipsNames_.disarmed.death});
}

TEST_F(CharacterControllerTests, DisarmedAttackAndWalkState_WalkChangeStateEvent)
{
    prepareState(*this);
    const auto& attackClipName = sut_.animationClipsNames_.disarmed.attack.front().name;
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<WalkChangeStateEvent>({attackClipName, sut_.animationClipsNames_.disarmed.run.forward});
}

TEST_F(CharacterControllerTests, DisarmedAttackAndWalkState_WeaponStateEvent)
{
    prepareState(*this);
    const auto& attackClipName = sut_.animationClipsNames_.disarmed.attack.front().name;
    tiggerAndExpect<WeaponStateEvent>({sut_.animationClipsNames_.disarmed.walk.forward, attackClipName});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.walk.forward, sut_.animationClipsNames_.equip});
}

TEST_F(CharacterControllerTests, DisarmedAttackAndWalkState_EndForwardMoveEvent)
{
    prepareState(*this);
    const auto& clip = sut_.animationClipsNames_.disarmed.attack.front();
    expectNoMove();
    tiggerAndExpect<EndForwardMoveEvent>({clip.name});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
}

TEST_F(CharacterControllerTests, DisarmedAttackAndWalkState_EndBackwardMoveEvent)
{
    prepareState(*this);
    const auto& clipName = sut_.animationClipsNames_.disarmed.attack.front().name;
    expectNoMove();
    tiggerAndExpect<MoveBackwardEvent>({clipName}, {0});
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<EndBackwardMoveEvent>({clipName, sut_.animationClipsNames_.disarmed.walk.forward}, {0});
    tiggerAndExpect<EndForwardMoveEvent>({clipName}, {0});
}

// TEST_F(CharacterControllerTests, DisarmedAttackAndWalkState_RotateLeftEvent)
//{
//     prepareState(*this);
//     const auto& clipName = sut_.animationClipsNames_.disarmed.attack.front().name;
//     expectRotationLeft();
//     tiggerAndExpect<RotateLeftEvent>({clipName, sut_.animationClipsNames_.disarmed.walk.forward});
// }

// TEST_F(CharacterControllerTests, DisarmedAttackAndWalkState_RotateRightEvent)
//{
//     prepareState(*this);
//     const auto& clipName = sut_.animationClipsNames_.disarmed.attack.front().name;
//     expectRotationRight();
//     tiggerAndExpect<RotateRightEvent>({clipName, sut_.animationClipsNames_.disarmed.walk.forward});
// }

// TEST_F(CharacterControllerTests, DisarmedAttackAndWalkState_RotateTargetEvent)
//{
//     prepareState(*this);
//     const auto& clipName = sut_.animationClipsNames_.disarmed.attack.front().name;
//     EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

//    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
//    tiggerAndExpect<RotateTargetEvent>(RotateTargetEvent{targetRotation.value_},
//                                       {clipName, sut_.animationClipsNames_.disarmed.walk.forward});
//}

TEST_F(CharacterControllerTests, DisarmedAttackAndWalkState_SprintStateChangeEvent)
{
    prepareState(*this);
    const auto& clipName = sut_.animationClipsNames_.disarmed.attack.front().name;
    tiggerAndExpect<SprintStateChangeEvent>({clipName, sut_.animationClipsNames_.disarmed.walk.forward});

    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.sprint});
}

TEST_F(CharacterControllerTests, DisarmedAttackAndWalkState_AttackEvent)
{
    prepareState(*this);
    const auto& clipName = sut_.animationClipsNames_.disarmed.attack.front().name;
    tiggerAndExpect<AttackEvent>({clipName, sut_.animationClipsNames_.disarmed.walk.forward});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet(
        {sut_.animationClipsNames_.disarmed.attack[1].name, sut_.animationClipsNames_.disarmed.walk.forward});
}
TEST_F(CharacterControllerTests, DisarmedAttackAndWalkState_AttackEventWhenRunBackward)
{
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    expectForwardVelocity(-DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveBackwardEvent>({sut_.animationClipsNames_.disarmed.run.forward});
    expectRootboneRotation(VECTOR_BACKWARD);
    expectForwardVelocity(-DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent>({sut_.animationClipsNames_.disarmed.walk.forward});

    expectNoMove();
    tiggerAndExpect<AttackEvent>({sut_.animationClipsNames_.disarmed.attack.front().name});
    expectForwardVelocity(-DEFAULT_WALK_SPEED);
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.walk.forward});
}
TEST_F(CharacterControllerTests, DisarmedAttackAndWalkState_MoveForwardAndLeftEvent)
{
    const auto& clip = sut_.animationClipsNames_.disarmed.attack.front();
    prepareState(*this);
    expectVelocity(VECTOR_FORWARD + VECTOR_LEFT, vec3(DEFAULT_WALK_SPEED, 0.0, DEFAULT_WALK_SPEED));
    tiggerAndExpect<MoveLeftEvent>({clip.name, sut_.animationClipsNames_.disarmed.walk.forward});
    expectVelocity(VECTOR_LEFT, vec3(DEFAULT_WALK_SPEED, 0.0, 0.0));
    tiggerAndExpect<EndForwardMoveEvent>({clip.name, sut_.animationClipsNames_.disarmed.walk.forward}, {0});
    tiggerAndExpect<EndMoveLeftEvent>({clip.name}, {0});
}
