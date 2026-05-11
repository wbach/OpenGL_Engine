#include "../CharacterControllerTests.h"
#include "Components/Controllers/CharacterController/CharacterControllerEvents.h"
#include "Components/Controllers/CharacterController/States/DisarmedIdleState.h"
#include "GameEngine/Physics/CollisionContactInfo.h"
#include "gmock/gmock.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.posture.stand.idle});
    test.tiggerAndExpect<HurtEvent>({test.sut_.animationClipsNames_.disarmed.posture.stand.hurt},
                                    {ADVANCED_TIME_TRANSITION_TIME});
}
}  // namespace

TEST_F(CharacterControllerTests, HurteState_BackToPrevious)
{
    prepareState(*this);

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, HurteState_DeathEvent)
{
    prepareState(*this);

    //EXPECT_CALL(physicsApiMock_, RemoveRigidBody(_));
    // Queue and wait
    tiggerAndExpect<DeathEvent>({sut_.animationClipsNames_.disarmed.posture.stand.death},
                               {ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
}
