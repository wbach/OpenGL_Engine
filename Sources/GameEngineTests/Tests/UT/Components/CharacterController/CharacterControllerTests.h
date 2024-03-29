#pragma once
#include <Utils/Time/Timer.h>
#include <gtest/gtest.h>

#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/Animation/JointPoseUpdater.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterController.h"
#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/Components/Physics/SphereShape.h"
#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"
#include "GameEngineTests/Tests/UT/Components/BaseComponent.h"

using namespace GameEngine;
using namespace GameEngine::Components;
using namespace ::testing;

namespace
{
const int DUMMY_FRAMES                    = 4;
const float DUMMY_FRAME_TIME_DELTA        = 0.25f;
const float DUMMY_CLIP_LENGTH             = DUMMY_FRAME_TIME_DELTA * static_cast<float>(DUMMY_FRAMES);
const float ADVANCED_TIME_TRANSITION_TIME = DEFAULT_ANIMATION_TRANSITION_TIME + 0.1f;
const float ADVANCED_TIME_CLIP_TIME       = DUMMY_CLIP_LENGTH + 0.1f;
}  // namespace

struct CharacterControllerTests : public BaseComponentTestSchould
{
    CharacterControllerTests();
    virtual void SetUp() override;
    virtual void TearDown() override;
    void createDummySkeleton();
    void addDummyClip(const std::string& name);
    void Update(float time);
    void expectAnimsToBeSet(const std::vector<std::string>& names);

    template <typename State>
    void expectState()
    {
        EXPECT_TRUE(std::holds_alternative<State*>(sut_.getFsm()->currentState));
    }

    template <typename Event, typename State>
    void tiggerAndExpect(const Event& event, const std::vector<std::string>& clipNames,
                         const std::vector<float>& updateTimes = {ADVANCED_TIME_TRANSITION_TIME})
    {
        sut_.handleEvent(event);
        for (auto updateTime : updateTimes)
            Update(updateTime);
        expectState<State>();
        expectAnimsToBeSet(clipNames);
    }

    template <typename Event, typename State>
    void tiggerAndExpect(const std::vector<std::string>& clipNames,
                         const std::vector<float>& updateTimes = {ADVANCED_TIME_TRANSITION_TIME})
    {
        tiggerAndExpect<Event, State>(Event{}, clipNames, updateTimes);
    }

    void expectForwardVelocity(float speed);
    void expectLeftVelocity(float);
    Rotation createRotaion(float deltaTime, float rotateSpeed);
    void expectRotatation(float deltaTime, float rotateSpeed);
    void expectRotationLeft(float dt = ADVANCED_TIME_TRANSITION_TIME);
    void expectRotationRight(float dt = ADVANCED_TIME_TRANSITION_TIME);

    CharacterController sut_;
    Animator* animator_{nullptr};
    Rigidbody* rigidbody_{nullptr};

    GameEngine::Model model_;
    GameEngine::Physics::ShapeId shapeId{1};
    GameEngine::Physics::RigidbodyId rigidbodyid{1};
};
