#include "../CharacterControllerTests.h"
#include "Components/Controllers/CharacterController/PlayStateType.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    EXPECT_CALL(test.physicsApiMock_, GetVelocity(test.rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(test.physicsApiMock_, GetRotation(test.rigidbodyid)).WillRepeatedly(Return(Rotation().value_));

    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.idle});
    test.tiggerAndExpect<WeaponStateEvent>(
        {test.sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
    test.tiggerAndExpect<AttackEvent>({test.sut_.animationClipsNames_.armed.attack.front().name},
                                      {ADVANCED_TIME_TRANSITION_TIME});
}
}  // namespace

TEST_F(CharacterControllerTests, ArmedAttackStateTests_DrawArrowEvent)
{
    prepareState(*this);
    tiggerAndExpect<DrawArrowEvent>({sut_.animationClipsNames_.drawArrow});
}

TEST_F(CharacterControllerTests, ArmedAttackStateTests_WeaponStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<WeaponStateEvent>({sut_.animationClipsNames_.disarm});
}

TEST_F(CharacterControllerTests, ArmedAttackStateTests_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent>({sut_.animationClipsNames_.armed.death});
}

TEST_F(CharacterControllerTests, ArmedAttackStateTests_MoveForwardEvent)
{
    auto& clip1     = sut_.animationClipsNames_.armed.attack[0];
    clip1.stateType = GameEngine::Components::PlayStateType::run;

    prepareState(*this);

    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveForwardEvent>({clip1.name, sut_.animationClipsNames_.armed.run.forward});
    expectRootboneRotation(VECTOR_FORWARD);
}

TEST_F(CharacterControllerTests, ArmedAttackStateTests_MoveBackwardEvent)
{
    auto& clip1     = sut_.animationClipsNames_.armed.attack[0];
    clip1.stateType = GameEngine::Components::PlayStateType::run;
    prepareState(*this);
    expectForwardVelocity(-DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveBackwardEvent>({clip1.name, sut_.animationClipsNames_.armed.run.forward});
    expectRootboneRotation(VECTOR_BACKWARD);
}

TEST_F(CharacterControllerTests, ArmedAttackStateTests_MoveLeftEvent)
{
    auto& clip1     = sut_.animationClipsNames_.armed.attack[0];
    clip1.stateType = GameEngine::Components::PlayStateType::run;
    prepareState(*this);
    expectVelocity(VECTOR_LEFT, vec3(DEFAULT_RUN_SPEED, 0.0, 0));
    tiggerAndExpect<MoveLeftEvent>({clip1.name, sut_.animationClipsNames_.armed.run.forward});
    expectRootboneRotation(VECTOR_LEFT);
}

TEST_F(CharacterControllerTests, ArmedAttackStateTests_MoveRightEvent)
{
    auto& clip1     = sut_.animationClipsNames_.armed.attack[0];
    clip1.stateType = GameEngine::Components::PlayStateType::run;
    prepareState(*this);
    expectVelocity(VECTOR_RIGHT, vec3(DEFAULT_RUN_SPEED, 0.0, 0));
    tiggerAndExpect<MoveRightEvent>({clip1.name, sut_.animationClipsNames_.armed.run.forward});
    expectRootboneRotation(VECTOR_RIGHT);
}

// TEST_F(CharacterControllerTests, ArmedAttackStateTests_RotateLeftEvent)
//{
//     prepareState(*this);
//     expectRotationLeft();
//     tiggerAndExpect<RotateLeftEvent>({sut_.animationClipsNames_.armed.rotateLeft});
// }

// TEST_F(CharacterControllerTests, ArmedAttackStateTests_RotateRightEvent)
//{
//     prepareState(*this);
//     expectRotationRight();
//     tiggerAndExpect<RotateRightEvent>({sut_.animationClipsNames_.armed.rotateRight});
// }

// TEST_F(CharacterControllerTests, ArmedAttackStateTests_RotateTargetEvent)
//{
//     prepareState(*this);

//    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

//    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
//    tiggerAndExpect<RotateTargetEvent>(RotateTargetEvent{targetRotation.value_},
//                                       {sut_.animationClipsNames_.armed.rotateLeft});
//}

TEST_F(CharacterControllerTests, ArmedAttackStateTests_AttackEvent)
{
    prepareState(*this);
    auto& clip     = sut_.animationClipsNames_.armed.attack[1];
    clip.stateType = GameEngine::Components::PlayStateType::run;

    tiggerAndExpect<AttackEvent>({sut_.animationClipsNames_.armed.attack.front().name}, {0});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    tiggerAndExpect<AttackEvent>({clip.name}, {0});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.attack[2].name});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.idle});
}

TEST_F(CharacterControllerTests, ArmedAttackStateTests_EndAttackEvent)
{
    prepareState(*this);
    tiggerAndExpect<EndAttackEvent>({sut_.animationClipsNames_.armed.idle}, {ADVANCED_TIME_TRANSITION_TIME});
}
