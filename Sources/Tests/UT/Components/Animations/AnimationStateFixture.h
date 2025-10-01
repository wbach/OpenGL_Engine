#pragma once
#include <gtest/gtest.h>

#include "GameEngine/Components/Animation/StateMachine.h"
#include "GameEngine/Components/Animation/Context.h"

using namespace GameEngine;
using namespace GameEngine::Components;

namespace AnimationStateTests
{
extern const float CLIP_LENGTH;
extern const float EPSILON;

extern const std::string lowerJointGroupName;
extern const std::string upperJointGroupName;
extern const std::string nonExistingJointGroupName;
extern const std::string incominClipName;

struct AnimationStateFixture : public ::testing::Test
{
    AnimationStateFixture();

    template <typename State>
    State* getStateIfActive()
    {
        return dynamic_cast<State*>(machine.currentState_.get());
    }

    AnimationClipInfo createClip(const std::string&, float = CLIP_LENGTH,
                                 AnimationClipInfo::PlayType = AnimationClipInfo::PlayType::once);
    void expectClipName(const std::string&);
    void expectClipName(const AnimationClipInfo&);
    void expectNoClipName(const std::string&);
    void expectNoClipName(const AnimationClipInfo&);

    Pose currentPose;
    JointGroupsIds jointGroups;
    StateMachine machine;

    Context context;
    AnimationClipInfo animationClipInfo;
};
}  // namespace AnimationStateTests
