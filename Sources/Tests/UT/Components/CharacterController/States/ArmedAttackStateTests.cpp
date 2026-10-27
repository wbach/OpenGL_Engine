#include "../CharacterControllerTests.h"
#include "Components/Controllers/CharacterController/PlayStateType.h"
#include "Logger/Log.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    EXPECT_CALL(test.physicsApiMock_, GetVelocity(test.rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(test.physicsApiMock_, GetRotation(test.rigidbodyid)).WillRepeatedly(Return(Rotation().value_));

    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.posture.stand.idle});
    test.triggerAndExpect<WeaponStateEvent>(
        {test.sut_.animationClipsNames_.armed.posture.stand.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
    test.triggerAndExpect<AttackEvent>({test.sut_.animationClipsNames_.armed.attack.front().clipsSequence.front()},
                                      {ADVANCED_TIME_TRANSITION_TIME});
}
}  // namespace

TEST_F(CharacterControllerTests, ArmedAttackStateTests_CrouchChangeStateEvent)
{
    prepareState(*this);

    triggerAndExpect<CrouchChangeStateEvent>({sut_.animationClipsNames_.armed.attack.front().clipsSequence.front()});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.posture.crouched.idle});
}

TEST_F(CharacterControllerTests, ArmedAttackStateTests_DrawArrowEvent)
{
    prepareState(*this);
    triggerAndExpect<DrawArrowEvent>({sut_.animationClipsNames_.armed.attack.front().clipsSequence.front()});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.aim.draw});
}

TEST_F(CharacterControllerTests, ArmedAttackStateTests_WeaponStateEvent)
{
    prepareState(*this);
    triggerAndExpect<WeaponStateEvent>({sut_.animationClipsNames_.disarm.front()});
}

TEST_F(CharacterControllerTests, ArmedAttackStateTests_DeathEvent)
{
    prepareState(*this);
    triggerAndExpect<DeathEvent>({sut_.animationClipsNames_.armed.posture.stand.death});
}

TEST_F(CharacterControllerTests, ArmedAttackStateTests_MoveForwardEvent)
{
    auto& clip1     = sut_.animationClipsNames_.armed.attack[0];
    clip1.stateType = GameEngine::Components::PlayStateType::run;

    prepareState(*this);

    expectForwardVelocity(DEFAULT_RUN_SPEED);
    triggerAndExpect<MoveForwardEvent>({clip1.clipsSequence.front(), sut_.animationClipsNames_.armed.movement.run.forward});
    expectRootboneRotation(VECTOR_FORWARD);
}

TEST_F(CharacterControllerTests, ArmedAttackStateTests_MoveBackwardEvent)
{
    auto& clip1     = sut_.animationClipsNames_.armed.attack[0];
    clip1.stateType = GameEngine::Components::PlayStateType::run;
    prepareState(*this);
    expectForwardVelocity(-DEFAULT_RUN_SPEED);
    triggerAndExpect<MoveBackwardEvent>({clip1.clipsSequence.front(), sut_.animationClipsNames_.armed.movement.run.forward});
    expectRootboneRotation(VECTOR_BACKWARD);
}

TEST_F(CharacterControllerTests, ArmedAttackStateTests_MoveLeftEvent)
{
    auto& clip1     = sut_.animationClipsNames_.armed.attack[0];
    clip1.stateType = GameEngine::Components::PlayStateType::run;
    prepareState(*this);
    expectVelocity(VECTOR_LEFT, vec3(DEFAULT_RUN_SPEED, 0.0, 0));
    triggerAndExpect<MoveLeftEvent>({clip1.clipsSequence.front(), sut_.animationClipsNames_.armed.movement.run.forward});
    expectRootboneRotation(VECTOR_LEFT);
}

TEST_F(CharacterControllerTests, ArmedAttackStateTests_MoveRightEvent)
{
    auto& clip1     = sut_.animationClipsNames_.armed.attack[0];
    clip1.stateType = GameEngine::Components::PlayStateType::run;
    prepareState(*this);
    expectVelocity(VECTOR_RIGHT, vec3(DEFAULT_RUN_SPEED, 0.0, 0));
    triggerAndExpect<MoveRightEvent>({clip1.clipsSequence.front(), sut_.animationClipsNames_.armed.movement.run.forward});
    expectRootboneRotation(VECTOR_RIGHT);
}

TEST_F(CharacterControllerTests, ArmedAttackStateTests_RotateLeftEvent)
{
    prepareState(*this);
    expectRotationLeft();
    triggerAndExpect<RotateLeftEvent>(
        {armedAttackClip1->clipsSequence.front(), sut_.animationClipsNames_.armed.posture.stand.rotate.left});
}

TEST_F(CharacterControllerTests, ArmedAttackStateTests_RotateRightEvent)
{
    prepareState(*this);
    expectRotationRight();
    triggerAndExpect<RotateRightEvent>(
        {armedAttackClip1->clipsSequence.front(), sut_.animationClipsNames_.armed.posture.stand.rotate.right});
}

TEST_F(CharacterControllerTests, ArmedAttackStateTests_RotateTargetEvent)
{
    prepareState(*this);

    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    triggerAndExpect<RotateTargetEvent>(
        RotateTargetEvent{targetRotation.value_},
        {armedAttackClip1->clipsSequence.front(), sut_.animationClipsNames_.armed.posture.stand.rotate.right});
}

TEST_F(CharacterControllerTests, ArmedAttackStateTests_AttackEvent)
{
    prepareState(*this);
    auto& clip     = sut_.animationClipsNames_.armed.attack[1];
    clip.stateType = GameEngine::Components::PlayStateType::run;

    triggerAndExpect<AttackEvent>({sut_.animationClipsNames_.armed.attack.front().clipsSequence.front()}, {0});

    Update(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    triggerAndExpect<AttackEvent>({clip.clipsSequence.front()}, {0});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.attack[2].clipsSequence.front()});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, ArmedAttackStateTests_EndAttackEvent)
{
    prepareState(*this);
    triggerAndExpect<EndAttackEvent>({sut_.animationClipsNames_.armed.posture.stand.idle}, {ADVANCED_TIME_TRANSITION_TIME});
}

TEST_F(CharacterControllerTests, ArmedAttackStateTests_OneSequnece)
{
    AnimSequence clipsSequence{.clipNames = {"A1Rise", "A1Hit", "A1Recovery"}};
    // prepareState(*this);
    auto& clips        = sut_.animationClipsNames_;
    clips.armed.attack = {AttackAnimation{.clipsSequence = clipsSequence, .stateType = PlayStateType::idle}};

    for (const auto& attackClip : clips.armed.attack)
    {
        for (auto& clipsSequene : attackClip.clipsSequence)
            addDummyClip(clipsSequene);
    }

    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    triggerAndExpect<WeaponStateEvent>({sut_.animationClipsNames_.armed.posture.stand.idle},
                                      {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    triggerAndExpect<AttackEvent>({clipsSequence[0]}, {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME});

    Update(ADVANCED_TIME_CLIP_TIME);
    expectAnimsToBeSet({clipsSequence[1]});

    Update(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({clipsSequence[2]});

    Update(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.posture.stand.idle});

    // triggerAndExpect<EndAttackEvent>({sut_.animationClipsNames_.armed.posture.stand.idle}, {ADVANCED_TIME_TRANSITION_TIME});
}

TEST_F(CharacterControllerTests, ArmedAttackStateTests_MoveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackStateTests_WalkChangeStateEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackStateTests_SprintStateChangeEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackStateTests_EndForwardMoveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackStateTests_EndBackwardMoveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackStateTests_EndMoveLeftEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackStateTests_EndMoveRightEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackStateTests_EndRotationEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackStateTests_JumpEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackStateTests_JumpConfirmEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackStateTests_DodgeDiveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackStateTests_DodgeForwardEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackStateTests_DodgeBackwardEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackStateTests_DodgeLeftEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackStateTests_DodgeRightEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackStateTests_DodgeEndEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackStateTests_EquipEndStateEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackStateTests_DisarmEndStateEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackStateTests_ReloadArrowEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackStateTests_AimStartEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackStateTests_AimStopEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackStateTests_StartFallingEvent)
{
    prepareState(*this);
    triggerAndExpect<StartFallingEvent>({sut_.animationClipsNames_.armed.falling});
}

TEST_F(CharacterControllerTests, ArmedAttackStateTests_GroundDetectionEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackStateTests_HurtEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackStateTests_AnimationPlayEndEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackStateTests_ChangeAnimEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackStateTests_StartDialogEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedAttackStateTests_EndDialogEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
