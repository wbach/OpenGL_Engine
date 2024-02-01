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
    virtual void SetUp() override
    {
        ASSERT_TRUE(sut_.fsm() != nullptr);
        EXPECT_CALL(physicsApiMock_, RemoveShape(shapeId));
        EXPECT_CALL(physicsApiMock_, RemoveRigidBody(rigidbodyid));

        Update(ADVANCED_TIME_TRANSITION_TIME);  // To set first anim
    }

    virtual void TearDown() override
    {
    }

    void createDummySkeleton()
    {
        animator_->jointGroups_.insert({sut_.lowerBodyGroupName, {"head", "neck", "leftHand", "rightHand"}});
        animator_->jointGroups_.insert({sut_.upperBodyGroupName, {"hips, leftLeg", "rightLeg"}});

        Animation::JointId id = 0;
        Animation::Joint rootJoint;
        rootJoint.name = "Armature";
        rootJoint.id   = id;

        for (const auto& [_, group] : animator_->jointGroups_)
        {
            for (const auto& jointName : group)
            {
                Animation::Joint joint;
                joint.name = jointName;
                joint.id   = ++id;
                rootJoint.addChild(joint);
            }
        }

        model_.setRootJoint(rootJoint);
    }

    void addDummyClip(const std::string& name)
    {
        Animation::AnimationClip clip(name);
        for (int i = 0; i <= DUMMY_FRAMES; ++i)
        {
            clip.AddFrame(Animation::KeyFrame{DUMMY_FRAME_TIME_DELTA * (float)i, {{0, Animation::JointTransform{}}}});
        }
        DEBUG_LOG("addDummyClip : " + name + " Length : " + std::to_string(clip.GetLength()));
        animator_->AddAnimationClip(clip);
    }

    void Update(float time)
    {
        DEBUG_LOG("Update");
        context_.time_.deltaTime = time;
        componentController_.CallFunctions(FunctionType::Update);
        sut_.Update();
    }

    void expectAnimsToBeSet(const std::vector<std::string>& names)
    {
        EXPECT_EQ(animator_->getCurrentAnimationName().size(), names.size());

        for (const auto& name : names)
        {
            DEBUG_LOG("Expected : " + name);
        }

        for (const auto& name : animator_->getCurrentAnimationName())
        {
            DEBUG_LOG("Current  : " + name);
            auto iter = std::find(names.begin(), names.end(), name);
            EXPECT_TRUE(iter != names.end());
        }
    }

    template <typename State>
    void expectState()
    {
        EXPECT_TRUE(std::holds_alternative<State*>(sut_.fsm()->currentState));
    }

    template <typename Event, typename State>
    void tiggerAndExpect(const std::vector<std::string>& clipNames,
                         std::vector<float> updateTimes = {ADVANCED_TIME_TRANSITION_TIME})
    {
        sut_.fsm()->handle(Event{});
        for (auto updateTime : updateTimes)
            Update(updateTime);
        expectState<State>();
        expectAnimsToBeSet(clipNames);
    }

    void expectForwardVelocity(float speed)
    {
        EXPECT_CALL(physicsApiMock_, SetVelocityRigidbody(rigidbodyid, vec3(0.0, 0.0, speed))).Times(AtLeast(1));
    }

    void expectRotatation(float deltaTime, float rotateSpeed)
    {
        auto rotation = glm::angleAxis(glm::radians(rotateSpeed * deltaTime), glm::vec3(0.f, 1.f, 0.f));

        DEBUG_LOG("Expected rotation : " + std::to_string(rotation));
        EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(Quaternion(rotation))))
            .Times(AtLeast(1));
    }

    void expectRotationLeft(float dt = ADVANCED_TIME_TRANSITION_TIME)
    {
        expectRotatation(dt, DEFAULT_TURN_SPEED);
    }

    void expectRotationRight(float dt = ADVANCED_TIME_TRANSITION_TIME)
    {
        expectRotatation(dt, -DEFAULT_TURN_SPEED);
    }

    CharacterController sut_;
    Animator* animator_{nullptr};
    Rigidbody* rigidbody_{nullptr};

    GameEngine::Model model_;
    GameEngine::Physics::ShapeId shapeId{1};
    GameEngine::Physics::RigidbodyId rigidbodyid{1};
};
