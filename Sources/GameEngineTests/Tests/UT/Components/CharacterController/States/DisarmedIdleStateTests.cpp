#include "../CharacterControllerTests.h"

TEST_F(CharacterControllerTests, DisarmedIdleState_DrawArrowEvent)
{
    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<DrawArrowEvent, ArmedIdleState>({sut_.animationClipsNames_.equip},
                                                    {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME / 2.f});
    Update(ADVANCED_TIME_CLIP_TIME / 2.f);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectState<DrawArrowState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.drawArrow});
}

TEST_F(CharacterControllerTests, DisarmedIdleState_AimStopDuringDrawArrowEvent)
{
    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<DrawArrowEvent, ArmedIdleState>({sut_.animationClipsNames_.equip},
                                                    {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME / 2.f});
    tiggerAndExpect<AimStopEvent, ArmedIdleState>({sut_.animationClipsNames_.equip});
    Update(ADVANCED_TIME_CLIP_TIME / 2.f);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectState<ArmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.idle});
}
