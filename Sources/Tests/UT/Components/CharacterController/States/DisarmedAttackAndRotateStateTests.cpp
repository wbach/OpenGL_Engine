#include "../CharacterControllerTests.h"
#include "Components/Controllers/CharacterController/CharacterControllerEvents.h"
#include "Components/Controllers/CharacterController/MoveSpeed.h"
#include "Types.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    EXPECT_CALL(test.physicsApiMock_, GetVelocity(test.rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(test.physicsApiMock_, GetRotation(test.rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    test.expectNoMove();

    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.posture.stand.idle});
    test.expectRotationLeft();
    test.tiggerAndExpect<RotateLeftEvent>({test.sut_.animationClipsNames_.disarmed.posture.stand.rotate.left});
    test.tiggerAndExpect<AttackEvent>(
        {test.disarmedAttackClip1->clipsSequence.front(), test.sut_.animationClipsNames_.disarmed.posture.stand.rotate.left});
}
}  // namespace

TEST_F(CharacterControllerTests, DisarmedAttackAndRotateState_DrawArrowEvent)
{
    prepareState(*this);
    expectRotationLeft();
    tiggerAndExpect<GameEngine::DrawArrowEvent>(
        {disarmedAttackClip1->clipsSequence.front(), sut_.animationClipsNames_.disarmed.posture.stand.rotate.left});

    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectRotationLeft(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.equip.front(), sut_.animationClipsNames_.armed.posture.stand.rotate.left});
}
TEST_F(CharacterControllerTests, DisarmedAttackAndRotateState_AimStop)
{
    prepareState(*this);
    expectRotationLeft();
    tiggerAndExpect<GameEngine::DrawArrowEvent>(
        {disarmedAttackClip1->clipsSequence.front(), sut_.animationClipsNames_.disarmed.posture.stand.rotate.left});
    expectAnyRotation();
    tiggerAndExpect<GameEngine::AimStopEvent>(
        {disarmedAttackClip1->clipsSequence.front(), sut_.animationClipsNames_.disarmed.posture.stand.rotate.left}, {0});
    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectRotationLeft(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.rotate.left});
}
TEST_F(CharacterControllerTests, DisarmedAttackAndRotateState_CrouchChangeStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<GameEngine::CrouchChangeStateEvent>(
        {disarmedAttackClip1->clipsSequence.front(), sut_.animationClipsNames_.disarmed.posture.stand.rotate.left});

    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectRotationLeft(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.crouched.rotate.left});
}
TEST_F(CharacterControllerTests, DisarmedAttackAndRotateState_MoveForwardEvent)
{
    prepareState(*this);
    expectRotationLeft();
    expectForwardVelocity(GameEngine::Components::DEFAULT_RUN_SPEED);
    tiggerAndExpect<GameEngine::MoveForwardEvent>(
        {disarmedAttackClip1->clipsSequence.front(), sut_.animationClipsNames_.disarmed.movement.run.forward});
}
TEST_F(CharacterControllerTests, DisarmedAttackAndRotateState_MoveBackwardEvent)
{
    prepareState(*this);
    expectAnyRotation();
    expectForwardVelocity(-GameEngine::Components::DEFAULT_RUN_SPEED);
    tiggerAndExpect<GameEngine::MoveBackwardEvent>(
        {disarmedAttackClip1->clipsSequence.front(), sut_.animationClipsNames_.disarmed.movement.run.forward});
    expectRootboneRotation(VECTOR_BACKWARD);
}
TEST_F(CharacterControllerTests, DisarmedAttackAndRotateState_MoveLeftEvent)
{
    prepareState(*this);
    expectAnyRotation();
    expectVelocity(VECTOR_LEFT, vec3(GameEngine::Components::DEFAULT_RUN_SPEED, 0.0, 0.0));
    tiggerAndExpect<GameEngine::MoveLeftEvent>(
        {disarmedAttackClip1->clipsSequence.front(), sut_.animationClipsNames_.disarmed.movement.run.forward});
    expectRootboneRotation(VECTOR_LEFT);
}
TEST_F(CharacterControllerTests, DisarmedAttackAndRotateState_MoveRighEvent)
{
    prepareState(*this);
    expectAnyRotation();
    expectVelocity(VECTOR_RIGHT, vec3(GameEngine::Components::DEFAULT_RUN_SPEED, 0.0, 0.0));
    tiggerAndExpect<GameEngine::MoveRightEvent>(
        {disarmedAttackClip1->clipsSequence.front(), sut_.animationClipsNames_.disarmed.movement.run.forward});
    expectRootboneRotation(VECTOR_RIGHT);
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRotateState_MoveRightEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRotateState_AimStopEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, DisarmedAttackAndRotateState_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent>({sut_.animationClipsNames_.disarmed.posture.stand.death});
}
TEST_F(CharacterControllerTests, DisarmedAttackAndRotateState_AttackEvent)
{
    prepareState(*this);
    tiggerAndExpect<AttackEvent>({sut_.animationClipsNames_.disarmed.attack.front().clipsSequence.front(),
                                  sut_.animationClipsNames_.disarmed.posture.stand.rotate.left});

    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectRotationLeft(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.attack[1].clipsSequence.front(),
                        sut_.animationClipsNames_.disarmed.posture.stand.rotate.left});

    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectRotationLeft(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.rotate.left});
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRotateState_EndRotationEvent)
{
    prepareState(*this);
    tiggerAndExpect<GameEngine::EndRotationEvent>({disarmedAttackClip1->clipsSequence.front()});
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRotateState_RotateLeftEvent)
{
    prepareState(*this);
    tiggerAndExpect<RotateLeftEvent>(
        {disarmedAttackClip1->clipsSequence.front(), sut_.animationClipsNames_.disarmed.posture.stand.rotate.left});
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRotateState_RotateRightEvent)
{
    prepareState(*this);
    expectRotationRight();
    tiggerAndExpect<RotateRightEvent>(
        {disarmedAttackClip1->clipsSequence.front(), sut_.animationClipsNames_.disarmed.posture.stand.rotate.right});
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRotateState_RotateTargetEvent)
{
    prepareState(*this);

    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent>(
        RotateTargetEvent{targetRotation.value_},
        {disarmedAttackClip1->clipsSequence.front(), sut_.animationClipsNames_.disarmed.posture.stand.rotate.right});
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRotateState_MoveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRotateState_WalkChangeStateEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRotateState_SprintStateChangeEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRotateState_EndForwardMoveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRotateState_EndBackwardMoveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRotateState_EndMoveLeftEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRotateState_EndMoveRightEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRotateState_JumpEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRotateState_JumpConfirmEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRotateState_DodgeDiveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRotateState_DodgeForwardEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRotateState_DodgeBackwardEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRotateState_DodgeLeftEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRotateState_DodgeRightEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRotateState_DodgeEndEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRotateState_EndAttackEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRotateState_EquipEndStateEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRotateState_DisarmEndStateEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRotateState_ReloadArrowEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRotateState_AimStartEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRotateState_StartFallingEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRotateState_GroundDetectionEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRotateState_HurtEvent)
{
    prepareState(*this);
    tiggerAndExpect<HurtEvent>({sut_.animationClipsNames_.disarmed.posture.stand.hurt});
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRotateState_AnimationPlayEndEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRotateState_ChangeAnimEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRotateState_StartDialogEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRotateState_EndDialogEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRotateState_WeaponStateEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
