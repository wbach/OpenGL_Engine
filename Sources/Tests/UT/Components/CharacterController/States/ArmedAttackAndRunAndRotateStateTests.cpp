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
    test.tiggerAndExpect<AttackEvent>({test.sut_.animationClipsNames_.armed.attack.front().clipsSequence.front(),
                                       test.sut_.animationClipsNames_.armed.movement.run.forward});
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
    const auto& clipName = sut_.animationClipsNames_.armed.attack.front().clipsSequence.front();
    tiggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.armed.posture.stand.rotate.left, clipName});
}

TEST_F(CharacterControllerTests, ArmedAttackAndRunAndRotateState_EndRotationEvent)
{
    prepareState(*this);
    expectNoRotation();
    const auto& clipName = sut_.animationClipsNames_.armed.attack.front().clipsSequence.front();
    tiggerAndExpect<EndRotationEvent>({clipName, sut_.animationClipsNames_.armed.movement.run.forward});
}

TEST_F(CharacterControllerTests, ArmedAttackAndRunAndRotateState_SprintStateChangeEvent)
{
    prepareState(*this);
    const auto& clipName = sut_.animationClipsNames_.armed.attack.front().clipsSequence.front();
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
    const auto& clipName = sut_.animationClipsNames_.armed.attack.front().clipsSequence.front();
    tiggerAndExpect<GameEngine::WeaponStateEvent>({clipName, sut_.animationClipsNames_.armed.movement.run.forward});

    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);

    expectRotationLeft(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.movement.run.forward, sut_.animationClipsNames_.disarm.front()});
}

TEST_F(CharacterControllerTests, ArmedAttackAndRunAndRotateState_DrawArrowEvent)
{
    prepareState(*this);
    const auto& clipName = sut_.animationClipsNames_.armed.attack.front().clipsSequence.front();
    tiggerAndExpect<GameEngine::DrawArrowEvent>({clipName, sut_.animationClipsNames_.armed.movement.run.forward});

    expectForwardVelocity(DEFAULT_WALK_SPEED);
    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);

    expectRotationLeft(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectRotationLeft(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.movement.walk.forward, sut_.animationClipsNames_.aim.draw});
}

TEST_F(CharacterControllerTests, ArmedAttackAndRunAndRotateState_AimStopEvent)
{
    prepareState(*this);
    const auto& clipName = sut_.animationClipsNames_.armed.attack.front().clipsSequence.front();
    tiggerAndExpect<GameEngine::DrawArrowEvent>({clipName, sut_.animationClipsNames_.armed.movement.run.forward});
    expectAnyRotation();
    tiggerAndExpect<GameEngine::AimStopEvent>({clipName, sut_.animationClipsNames_.armed.movement.run.forward}, {0});

    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);

    expectRotationLeft(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.movement.run.forward});
}

TEST_F(CharacterControllerTests, ArmedAttackAndRunAndRotateState_MoveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackAndRunAndRotateState_MoveBackwardEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackAndRunAndRotateState_MoveLeftEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackAndRunAndRotateState_MoveRightEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackAndRunAndRotateState_WalkChangeStateEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackAndRunAndRotateState_EndBackwardMoveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackAndRunAndRotateState_EndMoveLeftEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackAndRunAndRotateState_EndMoveRightEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackAndRunAndRotateState_RotateRightEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackAndRunAndRotateState_RotateTargetEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackAndRunAndRotateState_JumpEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackAndRunAndRotateState_JumpConfirmEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackAndRunAndRotateState_DodgeDiveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackAndRunAndRotateState_DodgeForwardEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackAndRunAndRotateState_DodgeBackwardEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackAndRunAndRotateState_DodgeLeftEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackAndRunAndRotateState_DodgeRightEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackAndRunAndRotateState_DodgeEndEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackAndRunAndRotateState_EquipEndStateEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackAndRunAndRotateState_DisarmEndStateEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackAndRunAndRotateState_ReloadArrowEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackAndRunAndRotateState_AimStartEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackAndRunAndRotateState_CrouchChangeStateEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackAndRunAndRotateState_StartFallingEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackAndRunAndRotateState_GroundDetectionEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackAndRunAndRotateState_HurtEvent)
{
    prepareState(*this);
    tiggerAndExpect<HurtEvent>({sut_.animationClipsNames_.armed.posture.stand.hurt});
}

TEST_F(CharacterControllerTests, ArmedAttackAndRunAndRotateState_AnimationPlayEndEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackAndRunAndRotateState_ChangeAnimEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackAndRunAndRotateState_StartDialogEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackAndRunAndRotateState_EndDialogEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
