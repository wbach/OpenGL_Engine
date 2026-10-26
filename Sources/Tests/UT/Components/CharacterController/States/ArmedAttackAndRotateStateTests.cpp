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
    test.tiggerAndExpect<WeaponStateEvent>(
        {test.sut_.animationClipsNames_.armed.posture.stand.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
    test.expectRotationLeft();
    test.tiggerAndExpect<RotateLeftEvent>({test.sut_.animationClipsNames_.armed.posture.stand.rotate.left});
    test.tiggerAndExpect<AttackEvent>(
        {test.armedAttackClip1->clipsSequence.front(), test.sut_.animationClipsNames_.armed.posture.stand.rotate.left});
}
}  // namespace

TEST_F(CharacterControllerTests, ArmedAttackAndRotateState_DrawArrowEvent)
{
    prepareState(*this);
    expectRotationLeft();
    tiggerAndExpect<GameEngine::DrawArrowEvent>(
        {armedAttackClip1->clipsSequence.front(), sut_.animationClipsNames_.armed.posture.stand.rotate.left});

    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectRotationLeft(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.aim.draw, sut_.animationClipsNames_.armed.posture.stand.rotate.left});
}
TEST_F(CharacterControllerTests, ArmedAttackAndRotateState_AimStop)
{
    prepareState(*this);
    expectRotationLeft();
    tiggerAndExpect<GameEngine::DrawArrowEvent>(
        {armedAttackClip1->clipsSequence.front(), sut_.animationClipsNames_.armed.posture.stand.rotate.left});
    expectAnyRotation();
    tiggerAndExpect<GameEngine::AimStopEvent>(
        {armedAttackClip1->clipsSequence.front(), sut_.animationClipsNames_.armed.posture.stand.rotate.left}, {0});
    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectRotationLeft(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.posture.stand.rotate.left});
}
TEST_F(CharacterControllerTests, ArmedAttackAndRotateState_CrouchChangeStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<GameEngine::CrouchChangeStateEvent>(
        {armedAttackClip1->clipsSequence.front(), sut_.animationClipsNames_.armed.posture.stand.rotate.left});

    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectRotationLeft(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.posture.crouched.rotate.left});
}
TEST_F(CharacterControllerTests, ArmedAttackAndRotateState_MoveForwardEvent)
{
    prepareState(*this);
    expectRotationLeft();
    expectForwardVelocity(GameEngine::Components::DEFAULT_RUN_SPEED);
    tiggerAndExpect<GameEngine::MoveForwardEvent>(
        {armedAttackClip1->clipsSequence.front(), sut_.animationClipsNames_.armed.movement.run.forward});
}
TEST_F(CharacterControllerTests, ArmedAttackAndRotateState_MoveBackwardEvent)
{
    prepareState(*this);
    expectAnyRotation();
    expectForwardVelocity(-GameEngine::Components::DEFAULT_RUN_SPEED);
    tiggerAndExpect<GameEngine::MoveBackwardEvent>(
        {armedAttackClip1->clipsSequence.front(), sut_.animationClipsNames_.armed.movement.run.forward});
    expectRootboneRotation(VECTOR_BACKWARD);
}
TEST_F(CharacterControllerTests, ArmedAttackAndRotateState_MoveLeftEvent)
{
    prepareState(*this);
    expectAnyRotation();
    expectVelocity(VECTOR_LEFT, vec3(GameEngine::Components::DEFAULT_RUN_SPEED, 0.0, 0.0));
    tiggerAndExpect<GameEngine::MoveLeftEvent>(
        {armedAttackClip1->clipsSequence.front(), sut_.animationClipsNames_.armed.movement.run.forward});
    expectRootboneRotation(VECTOR_LEFT);
}
TEST_F(CharacterControllerTests, ArmedAttackAndRotateState_MoveRighEvent)
{
    prepareState(*this);
    expectAnyRotation();
    expectVelocity(VECTOR_RIGHT, vec3(GameEngine::Components::DEFAULT_RUN_SPEED, 0.0, 0.0));
    tiggerAndExpect<GameEngine::MoveRightEvent>(
        {armedAttackClip1->clipsSequence.front(), sut_.animationClipsNames_.armed.movement.run.forward});
    expectRootboneRotation(VECTOR_RIGHT);
}
TEST_F(CharacterControllerTests, ArmedAttackAndRotateState_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent>({sut_.animationClipsNames_.armed.posture.stand.death});
}
TEST_F(CharacterControllerTests, ArmedAttackAndRotateState_AttackEvent)
{
    prepareState(*this);
    tiggerAndExpect<AttackEvent>({sut_.animationClipsNames_.armed.attack.front().clipsSequence.front(),
                                  sut_.animationClipsNames_.armed.posture.stand.rotate.left});

    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectRotationLeft(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.attack[1].clipsSequence.front(),
                        sut_.animationClipsNames_.armed.posture.stand.rotate.left});

    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectRotationLeft(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.posture.stand.rotate.left});
}

TEST_F(CharacterControllerTests, ArmedAttackAndRotateState_EndRotationEvent)
{
    prepareState(*this);
    tiggerAndExpect<GameEngine::EndRotationEvent>({armedAttackClip1->clipsSequence.front()});
}

TEST_F(CharacterControllerTests, ArmedAttackAndRotateState_RotateLeftEvent)
{
    prepareState(*this);
    tiggerAndExpect<RotateLeftEvent>(
        {armedAttackClip1->clipsSequence.front(), sut_.animationClipsNames_.armed.posture.stand.rotate.left});
}

TEST_F(CharacterControllerTests, ArmedAttackAndRotateState_RotateRightEvent)
{
    prepareState(*this);
    expectRotationRight();
    tiggerAndExpect<RotateRightEvent>(
        {armedAttackClip1->clipsSequence.front(), sut_.animationClipsNames_.armed.posture.stand.rotate.right});
}

TEST_F(CharacterControllerTests, ArmedAttackAndRotateState_RotateTargetEvent)
{
    prepareState(*this);

    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent>(
        RotateTargetEvent{targetRotation.value_},
        {armedAttackClip1->clipsSequence.front(), sut_.animationClipsNames_.armed.posture.stand.rotate.right});
}

TEST_F(CharacterControllerTests, ArmedAttackAndRotateState_MoveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackAndRotateState_MoveRightEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackAndRotateState_WalkChangeStateEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackAndRotateState_SprintStateChangeEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackAndRotateState_EndForwardMoveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackAndRotateState_EndBackwardMoveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackAndRotateState_EndMoveLeftEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackAndRotateState_EndMoveRightEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackAndRotateState_JumpEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackAndRotateState_JumpConfirmEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackAndRotateState_DodgeDiveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackAndRotateState_DodgeForwardEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackAndRotateState_DodgeBackwardEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackAndRotateState_DodgeLeftEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackAndRotateState_DodgeRightEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackAndRotateState_DodgeEndEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackAndRotateState_EndAttackEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackAndRotateState_EquipEndStateEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackAndRotateState_DisarmEndStateEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackAndRotateState_ReloadArrowEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackAndRotateState_AimStartEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackAndRotateState_AimStopEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackAndRotateState_StartFallingEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackAndRotateState_GroundDetectionEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackAndRotateState_HurtEvent)
{
    prepareState(*this);
    tiggerAndExpect<HurtEvent>({sut_.animationClipsNames_.armed.posture.stand.hurt});
}

TEST_F(CharacterControllerTests, ArmedAttackAndRotateState_AnimationPlayEndEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackAndRotateState_ChangeAnimEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackAndRotateState_StartDialogEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackAndRotateState_EndDialogEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, ArmedAttackAndRotateState_WeaponStateEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
