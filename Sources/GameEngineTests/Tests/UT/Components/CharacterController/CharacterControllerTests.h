#pragma once
#include <Utils/Time/Timer.h>
#include <gtest/gtest.h>

#include "Components/Controllers/CharacterController/AnimationClipNames.h"
#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/Animation/BowPoseUpdater.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterController.h"
#include "GameEngine/Components/Controllers/CharacterController/MoveSpeed.h"
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
    template <typename Event>
    void tiggerAndExpect(const Event& event, const std::vector<std::string>& clipNames,
                         const std::vector<float>& updateTimes = {ADVANCED_TIME_TRANSITION_TIME})
    {
        sut_.handleEvent(event);
        for (auto updateTime : updateTimes)
            Update(updateTime);
        expectAnimsToBeSet(clipNames);
    }

    template <typename Event>
    void tiggerAndExpect(const std::vector<std::string>& clipNames,
                         const std::vector<float>& updateTimes = {ADVANCED_TIME_TRANSITION_TIME}, const Event& event = Event{})
    {
        tiggerAndExpect<Event>(event, clipNames, updateTimes);
    }

    void expectNoMove();
    void expectNoRotation();
    void expectVelocity(const vec3& dir, const vec3& moveSpeed, const vec3& currentVelocity = vec3(0));
    void expectForwardVelocity(float speed);
    void expectLeftVelocity(float);
    Rotation createRotaion(float deltaTime, float rotateSpeed);
    void expectAnyRotation();
    void expectRotatation(float deltaTime, float rotateSpeed);
    void expectRotationLeft(float dt = ADVANCED_TIME_TRANSITION_TIME);
    void expectRotationRight(float dt = ADVANCED_TIME_TRANSITION_TIME);
    void expectRootboneRotation(const vec3&);

    CharacterController sut_;
    Animator* animator_{nullptr};
    Rigidbody* rigidbody_{nullptr};

    GameEngine::Model model_;
    GameEngine::Physics::ShapeId shapeId{1};
    GameEngine::Physics::RigidbodyId rigidbodyid{1};

    AttackAnimation* armedAttackClip1;
    AttackAnimation* armedAttackClip2;
    AttackAnimation* armedAttackClip3;
    AttackAnimation* disarmedAttackClip1;
    AttackAnimation* disarmedAttackClip2;
    AttackAnimation* disarmedAttackClip3;

    vec3 lastSetVelocity = vec3(0);
};
