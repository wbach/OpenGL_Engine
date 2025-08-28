#include "../CharacterControllerTests.h"
#include "Components/Controllers/CharacterController/CharacterControllerEvents.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    test.sut_.animationClipsNames_.disarmed.attack.front().stateType = GameEngine::Components::PlayStateType::run;

    EXPECT_CALL(test.physicsApiMock_, GetVelocity(test.rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(test.physicsApiMock_, GetRotation(test.rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    test.expectForwardVelocity(DEFAULT_RUN_SPEED);

    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.posture.stand.idle});
    test.tiggerAndExpect<WeaponStateEvent>(
        {test.sut_.animationClipsNames_.armed.posture.stand.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
    test.tiggerAndExpect<MoveForwardEvent>({test.sut_.animationClipsNames_.armed.movement.run.forward});
    test.expectRotationLeft();
    test.tiggerAndExpect<RotateLeftEvent>({test.sut_.animationClipsNames_.armed.movement.run.forward});
    test.tiggerAndExpect<AttackEvent>(
        {test.sut_.animationClipsNames_.armed.attack.front().name, test.sut_.animationClipsNames_.armed.movement.run.forward});
}
}  // namespace

TEST_F(CharacterControllerTests, ArmedAttackAndRunAndRotateState_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent>({sut_.animationClipsNames_.armed.posture.stand.death});
}

TEST_F(CharacterControllerTests, ArmedAttackAndRunAndRotateState_EndAttackEvent)
{
    prepareState(*this);
    tiggerAndExpect<EndAttackEvent>({sut_.animationClipsNames_.armed.movement.run.forward});
}

TEST_F(CharacterControllerTests, ArmedAttackAndRunAndRotateState_EndMoveForwardEvent)
{
    prepareState(*this);
    expectNoMove();
    const auto& clipName = sut_.animationClipsNames_.armed.attack.front().name;
    tiggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.armed.posture.stand.rotate.left, clipName});
}

TEST_F(CharacterControllerTests, ArmedAttackAndRunAndRotateState_EndRotationEvent)
{
    prepareState(*this);
    expectNoRotation();
    const auto& clipName = sut_.animationClipsNames_.armed.attack.front().name;
    tiggerAndExpect<EndRotationEvent>({clipName, sut_.animationClipsNames_.armed.movement.run.forward});
}

TEST_F(CharacterControllerTests, ArmedAttackAndRunAndRotateState_SprintStateChangeEvent)
{
    prepareState(*this);
    const auto& clipName = sut_.animationClipsNames_.armed.attack.front().name;
    tiggerAndExpect<SprintStateChangeEvent>({clipName, sut_.animationClipsNames_.armed.movement.run.forward});

    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);

    expectRotationLeft(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.sprint});
}

TEST_F(CharacterControllerTests, ArmedAttackAndRunAndRotateState_WeaponStateEvent)
{
    prepareState(*this);
    const auto& clipName = sut_.animationClipsNames_.armed.attack.front().name;
    tiggerAndExpect<GameEngine::WeaponStateEvent>({clipName, sut_.animationClipsNames_.armed.movement.run.forward});

    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);

    expectRotationLeft(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.movement.run.forward, sut_.animationClipsNames_.disarm});
}

TEST_F(CharacterControllerTests, ArmedAttackAndRunAndRotateState_DrawArrowEvent)
{
    prepareState(*this);
    const auto& clipName = sut_.animationClipsNames_.armed.attack.front().name;
    tiggerAndExpect<GameEngine::DrawArrowEvent>({clipName, sut_.animationClipsNames_.armed.movement.run.forward});

    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);

    expectRotationLeft(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectForwardVelocity(DEFAULT_WALK_SPEED);
    expectRotationLeft(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.movement.walk.forward, sut_.animationClipsNames_.aim.draw});
}

TEST_F(CharacterControllerTests, ArmedAttackAndRunAndRotateState_AimStopEvent)
{
    prepareState(*this);
    const auto& clipName = sut_.animationClipsNames_.armed.attack.front().name;
    tiggerAndExpect<GameEngine::DrawArrowEvent>({clipName, sut_.animationClipsNames_.armed.movement.run.forward});
    expectAnyRotation();
    tiggerAndExpect<GameEngine::AimStopEvent>({clipName, sut_.animationClipsNames_.armed.movement.run.forward}, {0});

    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);

    expectRotationLeft(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.movement.run.forward});
}
