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
    test.tiggerAndExpect<MoveForwardEvent>({test.sut_.animationClipsNames_.disarmed.run.forward});
    test.expectRotationLeft();
    test.tiggerAndExpect<RotateLeftEvent>({test.sut_.animationClipsNames_.disarmed.run.forward});
    test.tiggerAndExpect<AttackEvent>({test.sut_.animationClipsNames_.disarmed.attack.front().name,
                                       test.sut_.animationClipsNames_.disarmed.run.forward});
}
}  // namespace

TEST_F(CharacterControllerTests, DisarmedAttackAndRunAndRotateState_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent>({sut_.animationClipsNames_.disarmed.death});
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunAndRotateState_EndAttackEvent)
{
    prepareState(*this);
    tiggerAndExpect<EndAttackEvent>({sut_.animationClipsNames_.disarmed.run.forward});
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunAndRotateState_EndMoveForwardEvent)
{
    prepareState(*this);
    expectNoMove();
    const auto& clipName = sut_.animationClipsNames_.disarmed.attack.front().name;
    tiggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.disarmed.rotateLeft, clipName});
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunAndRotateState_EndRotationEvent)
{
    prepareState(*this);
    expectNoRotation();
    const auto& clipName = sut_.animationClipsNames_.disarmed.attack.front().name;
    tiggerAndExpect<EndRotationEvent>({clipName, sut_.animationClipsNames_.disarmed.run.forward});
}
