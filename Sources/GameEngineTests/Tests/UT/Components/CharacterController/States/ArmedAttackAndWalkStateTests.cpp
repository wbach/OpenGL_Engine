#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    test.sut_.animationClipsNames_.disarmed.attack.front().stateType =
        GameEngine::Components::AttackAnimation::PlayStateType::run;

    EXPECT_CALL(test.physicsApiMock_, GetVelocity(test.rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(test.physicsApiMock_, GetRotation(test.rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    test.expectForwardVelocity(DEFAULT_RUN_SPEED);

    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.idle});
    test.tiggerAndExpect<WeaponStateEvent>(
        {test.sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    test.tiggerAndExpect<MoveForwardEvent>({test.sut_.animationClipsNames_.armed.run.forward});
    test.expectForwardVelocity(DEFAULT_WALK_SPEED);
    test.tiggerAndExpect<WalkChangeStateEvent>({test.sut_.animationClipsNames_.armed.walk.forward});

    test.tiggerAndExpect<AttackEvent>(
        {test.sut_.animationClipsNames_.armed.attack.front().name, test.sut_.animationClipsNames_.armed.walk.forward});
}
}  // namespace

TEST_F(CharacterControllerTests, ArmedAttackAndWalkState_DrawArrowEvent)
{
    prepareState(*this);
    // queue DrawArrowEvent
    tiggerAndExpect<DrawArrowEvent>(
        {sut_.animationClipsNames_.armed.attack.front().name, sut_.animationClipsNames_.armed.walk.forward});

    // wait until attack finish
    Update(ADVANCED_TIME_CLIP_TIME);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.walk.forward, sut_.animationClipsNames_.drawArrow});
}

TEST_F(CharacterControllerTests, ArmedAttackAndWalkState_MoveForwardEvent)
{
    prepareState(*this);
    const auto& attackClipName = sut_.animationClipsNames_.armed.attack.front().name;

    tiggerAndExpect<EndForwardMoveEvent>({attackClipName});

    expectForwardVelocity(-DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveBackwardEvent>({attackClipName, sut_.animationClipsNames_.armed.run.forward}, {0});
    expectRootboneRotation(VECTOR_BACKWARD);
    expectForwardVelocity(-DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent>({attackClipName, sut_.animationClipsNames_.armed.walk.forward});

    expectNoMove();
    tiggerAndExpect<MoveForwardEvent>({attackClipName});
}

TEST_F(CharacterControllerTests, ArmedAttackAndWalkState_MoveBackwardEvent)
{
    const auto& attackClipName = sut_.animationClipsNames_.armed.attack.front().name;
    prepareState(*this);
    expectNoMove();
    tiggerAndExpect<MoveBackwardEvent>({attackClipName});
}

TEST_F(CharacterControllerTests, ArmedAttackAndWalkState_MoveLeftEvent_EndMoveLeftEvent)
{
    prepareState(*this);
    const auto& attackClipName = sut_.animationClipsNames_.armed.attack.front().name;

    expectVelocity(VECTOR_FORWARD + VECTOR_LEFT, vec3(DEFAULT_WALK_SPEED, 0.0, DEFAULT_WALK_SPEED));
    tiggerAndExpect<MoveLeftEvent>({attackClipName, sut_.animationClipsNames_.armed.walk.forward}, {0});
    expectRootboneRotation(VECTOR_FORWARD + VECTOR_LEFT);
    expectVelocity(VECTOR_LEFT, vec3(DEFAULT_WALK_SPEED, 0.0, 0.0));
    tiggerAndExpect<EndForwardMoveEvent>({attackClipName, sut_.animationClipsNames_.armed.walk.forward}, {0});
    expectRootboneRotation(VECTOR_LEFT);
    tiggerAndExpect<EndMoveLeftEvent>({attackClipName}, {0});
}

TEST_F(CharacterControllerTests, ArmedAttackAndWalkState_MoveRightEvent_EndMoveRightEvent)
{
    prepareState(*this);
    const auto& attackClipName = sut_.animationClipsNames_.armed.attack.front().name;

    expectVelocity(VECTOR_FORWARD + VECTOR_RIGHT, vec3(DEFAULT_WALK_SPEED, 0.0, DEFAULT_WALK_SPEED));
    tiggerAndExpect<MoveRightEvent>({attackClipName, sut_.animationClipsNames_.armed.walk.forward}, {0});
    expectRootboneRotation(VECTOR_FORWARD + VECTOR_RIGHT);
    expectVelocity(VECTOR_RIGHT, vec3(DEFAULT_WALK_SPEED, 0.0, 0.0));
    tiggerAndExpect<EndForwardMoveEvent>({attackClipName, sut_.animationClipsNames_.armed.walk.forward}, {0});
    expectRootboneRotation(VECTOR_RIGHT);
    tiggerAndExpect<EndMoveRightEvent>({attackClipName}, {0});
}

TEST_F(CharacterControllerTests, ArmedAttackAndWalkState_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent>({sut_.animationClipsNames_.armed.death});
}

TEST_F(CharacterControllerTests, ArmedAttackAndWalkState_WalkChangeStateEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    const auto& attackClipName = sut_.animationClipsNames_.armed.attack.front().name;

    tiggerAndExpect<WalkChangeStateEvent>({attackClipName, sut_.animationClipsNames_.armed.run.forward});
}

TEST_F(CharacterControllerTests, ArmedAttackAndWalkState_WeaponStateEvent)
{
    prepareState(*this);
    const auto& attackClipName = sut_.animationClipsNames_.armed.attack.front().name;
    tiggerAndExpect<WeaponStateEvent>({sut_.animationClipsNames_.armed.walk.forward, attackClipName});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.walk.forward, sut_.animationClipsNames_.disarm});
}

TEST_F(CharacterControllerTests, ArmedAttackAndWalkState_EndForwardMoveEvent)
{
    prepareState(*this);
    const auto& attackClipName = sut_.animationClipsNames_.armed.attack.front().name;
    tiggerAndExpect<EndForwardMoveEvent>({attackClipName});
}

TEST_F(CharacterControllerTests, ArmedAttackAndWalkState_EndBackwardMoveEvent)
{
    prepareState(*this);
    const auto& clipName = sut_.animationClipsNames_.armed.attack.front().name;
    expectNoMove();
    tiggerAndExpect<MoveBackwardEvent>({clipName}, {0});
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<EndBackwardMoveEvent>({clipName, sut_.animationClipsNames_.armed.walk.forward}, {0});
    tiggerAndExpect<EndForwardMoveEvent>({clipName}, {0});
}

// TEST_F(CharacterControllerTests, ArmedAttackAndWalkState_RotateLeftEvent)
//{
//     prepareState(*this);
//     expectRotationLeft();
//     tiggerAndExpect<RotateLeftEvent>({sut_.animationClipsNames_.armed.run.forward});
// }

// TEST_F(CharacterControllerTests, ArmedAttackAndWalkState_RotateRightEvent)
//{
//     prepareState(*this);
//     expectRotationRight();
//     tiggerAndExpect<RotateRightEvent>({sut_.animationClipsNames_.armed.run.forward});
// }

// TEST_F(CharacterControllerTests, ArmedAttackAndWalkState_RotateTargetEvent)
//{
//     prepareState(*this);
//     EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

//    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
//    tiggerAndExpect<RotateTargetEvent>(RotateTargetEvent{targetRotation.value_},
//                                       {sut_.animationClipsNames_.armed.run.forward});
//}

TEST_F(CharacterControllerTests, ArmedAttackAndWalkState_SprintStateChangeEvent)
{
    prepareState(*this);
    const auto& clipName = sut_.animationClipsNames_.armed.attack.front().name;
    tiggerAndExpect<SprintStateChangeEvent>({clipName, sut_.animationClipsNames_.armed.walk.forward});

    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.sprint});
}

TEST_F(CharacterControllerTests, ArmedAttackAndWalkState_AttackEvent)
{
    prepareState(*this);
    const auto& clipName = sut_.animationClipsNames_.armed.attack.front().name;
    tiggerAndExpect<AttackEvent>({clipName, sut_.animationClipsNames_.armed.walk.forward});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.attack[1].name, sut_.animationClipsNames_.armed.walk.forward});
}
TEST_F(CharacterControllerTests, ArmedAttackAndWalkState_AttackEventWhenWalkBackward)
{
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    expectForwardVelocity(-DEFAULT_RUN_SPEED);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<WeaponStateEvent>(
        {sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    tiggerAndExpect<MoveBackwardEvent>({sut_.animationClipsNames_.armed.run.forward});
    expectForwardVelocity(-DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent>({sut_.animationClipsNames_.armed.walk.forward});

    expectNoMove();
    tiggerAndExpect<AttackEvent>({sut_.animationClipsNames_.armed.attack.front().name});
    expectForwardVelocity(-DEFAULT_WALK_SPEED);
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.walk.forward});
}
//TEST_F(CharacterControllerTests, ArmedAttackAndWalkState_EndMoveEventWhenAttackEventIsProcessing) // TO DO : should stop if anim needed
//    prepareState(*this);
//    sut_.animationClipsNames_.disarmed.attack.front().stateType =
//        GameEngine::Components::AttackAnimation::PlayStateType::idle;
//    expectNoMove();
//    tiggerAndExpect<AttackEvent>(
//        {sut_.animationClipsNames_.armed.attack.front().name});
//    tiggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.armed.attack.front().name});
//    Update(ADVANCED_TIME_CLIP_TIME);
//    Update(ADVANCED_TIME_TRANSITION_TIME);
//    expectAnimsToBeSet({sut_.animationClipsNames_.armed.idle});
//}
