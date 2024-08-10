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

    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.idle});
    test.expectRotationLeft();
    test.tiggerAndExpect<RotateLeftEvent>({test.sut_.animationClipsNames_.disarmed.rotateLeft});
    test.tiggerAndExpect<AttackEvent>({test.disarmedAttackClip1->name, test.sut_.animationClipsNames_.disarmed.rotateLeft});
}
}  // namespace

TEST_F(CharacterControllerTests, DisarmedAttackAndRotateState_DrawArrowEvent)
{
    prepareState(*this);
    expectRotationLeft();
    tiggerAndExpect<GameEngine::DrawArrowEvent>({disarmedAttackClip1->name, sut_.animationClipsNames_.disarmed.rotateLeft});

    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectRotationLeft(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.rotateLeft});
}
TEST_F(CharacterControllerTests, DisarmedAttackAndRotateState_AimStop)
{
    prepareState(*this);
    expectRotationLeft();
    tiggerAndExpect<GameEngine::DrawArrowEvent>({disarmedAttackClip1->name, sut_.animationClipsNames_.disarmed.rotateLeft});
    expectAnyRotation();
    tiggerAndExpect<GameEngine::AimStopEvent>({disarmedAttackClip1->name, sut_.animationClipsNames_.disarmed.rotateLeft}, {0});
    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectRotationLeft(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.rotateLeft});
}
TEST_F(CharacterControllerTests, DISABLED_DisarmedAttackAndRotateState_CrouchChangeStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<GameEngine::CrouchChangeStateEvent>(
        {disarmedAttackClip1->name, sut_.animationClipsNames_.disarmed.rotateLeft});

    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectRotationLeft(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    // expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.crouch.rotateLeft});
}
TEST_F(CharacterControllerTests, DisarmedAttackAndRotateState_MoveForwardEvent)
{
    prepareState(*this);
    expectRotationLeft();
    expectForwardVelocity(GameEngine::Components::DEFAULT_RUN_SPEED);
    tiggerAndExpect<GameEngine::MoveForwardEvent>({disarmedAttackClip1->name, sut_.animationClipsNames_.disarmed.run.forward});
}
TEST_F(CharacterControllerTests, DisarmedAttackAndRotateState_MoveBackwardEvent)
{
    prepareState(*this);
    expectAnyRotation();
    expectForwardVelocity(-GameEngine::Components::DEFAULT_RUN_SPEED);
    tiggerAndExpect<GameEngine::MoveBackwardEvent>({disarmedAttackClip1->name, sut_.animationClipsNames_.disarmed.run.forward});
    expectRootboneRotation(VECTOR_BACKWARD);
}
TEST_F(CharacterControllerTests, DisarmedAttackAndRotateState_MoveLeftEvent)
{
    prepareState(*this);
    expectAnyRotation();
    expectVelocity(VECTOR_LEFT, vec3(GameEngine::Components::DEFAULT_RUN_SPEED, 0.0, 0.0));
    tiggerAndExpect<GameEngine::MoveLeftEvent>({disarmedAttackClip1->name, sut_.animationClipsNames_.disarmed.run.forward});
    expectRootboneRotation(VECTOR_LEFT);
}
TEST_F(CharacterControllerTests, DisarmedAttackAndRotateState_MoveRighEvent)
{
    prepareState(*this);
    expectAnyRotation();
    expectVelocity(VECTOR_RIGHT, vec3(GameEngine::Components::DEFAULT_RUN_SPEED, 0.0, 0.0));
    tiggerAndExpect<GameEngine::MoveRightEvent>({disarmedAttackClip1->name, sut_.animationClipsNames_.disarmed.run.forward});
    expectRootboneRotation(VECTOR_RIGHT);
}
TEST_F(CharacterControllerTests, DisarmedAttackAndRotateState_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent>({sut_.animationClipsNames_.disarmed.death});
}
TEST_F(CharacterControllerTests, DisarmedAttackAndRotateState_AttackEvent)
{
    prepareState(*this);
    tiggerAndExpect<AttackEvent>(
        {sut_.animationClipsNames_.disarmed.attack.front().name, sut_.animationClipsNames_.disarmed.rotateLeft});

    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectRotationLeft(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.attack[1].name, sut_.animationClipsNames_.disarmed.rotateLeft});

    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectRotationLeft(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.rotateLeft});
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRotateState_EndRotationEvent)
{
    prepareState(*this);
    tiggerAndExpect<GameEngine::EndRotationEvent>({disarmedAttackClip1->name});
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRotateState_RotateLeftEvent)
{
    prepareState(*this);
    tiggerAndExpect<RotateLeftEvent>({disarmedAttackClip1->name, sut_.animationClipsNames_.disarmed.rotateLeft});
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRotateState_RotateRightEvent)
{
    prepareState(*this);
    expectRotationRight();
    tiggerAndExpect<RotateRightEvent>({disarmedAttackClip1->name, sut_.animationClipsNames_.disarmed.rotateRight});
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRotateState_RotateTargetEvent)
{
    prepareState(*this);

    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent>(RotateTargetEvent{targetRotation.value_},
                                       {disarmedAttackClip1->name, sut_.animationClipsNames_.disarmed.rotateLeft});
}