#include <gtest/gtest.h>

#include <string>

#include "Components/Animation/Animator.h"
#include "Components/ComponentsReadFunctions.h"
#include "Components/Renderer/Entity/RendererComponent.hpp"
#include "Logger/Log.h"
#include "Objects/GameObject.h"
#include "Scene/Scene.hpp"
#include "Tests/UT/EngineBasedTest.h"
#include "TreeNode.h"
#include "magic_enum/magic_enum.hpp"

using namespace GameEngine;
using namespace GameEngine::Components;

using namespace testing;

namespace
{
constexpr char model[] = "testModel.obj";
constexpr char clip[]  = "testClip";

class TestComponent : public BaseComponent
{
public:
    TestComponent(ComponentContext& componentContext, GameObject& gameObject)
        : BaseComponent(GetComponentType<TestComponent>(), componentContext, gameObject)
    {
    }

    // TO DO check register functon are properly invoked
    void ReqisterFunctions() override
    {
        RegisterFunction(FunctionType::Awake, [this]() { onAwakendCalled = true; });
        RegisterFunction(FunctionType::OnStart, [this]() { onStartCalled = true; });
    }
    void CleanUp() override
    {
    }
    void Reload() override
    {
    }

    bool onAwakendCalled{false};
    bool onStartCalled{false};
};
}  // namespace

struct GameObjectTestSchould : public EngineBasedTest
{
    GameObjectTestSchould()
    {
    }
    void SetUp() override
    {
        EngineBasedTest::SetUp();
    }

    void verifyComponents()
    {
        auto rendererComponent = sut_->GetComponent<RendererComponent>();
        auto animator          = sut_->GetComponent<Animator>();

        EXPECT_TRUE(animator != nullptr);
        EXPECT_TRUE(rendererComponent != nullptr);

        EXPECT_EQ(model, rendererComponent->fileName_LOD1);
        EXPECT_NE(std::find_if(animator->animationClips.begin(), animator->animationClips.end(),
                               [&expectedClip = clip](const auto& clip) { return clip.name == expectedClip; }),
                  animator->animationClips.end());
    }

    void CreateSut()
    {
        auto go = scene->CreateGameObject("SUT");
        sut_    = go.get();
        scene->AddGameObject(std::move(go));
        scene->ProcessEvents();
    }

    template <typename ComponentType>
    void CheckComponentRegistration()
    {
        auto rendererComponents = scene->getComponentController().GetAllComponentsOfType<ComponentType>();
        EXPECT_FALSE(rendererComponents.empty());
    }

    GameObject* sut_;
};

TEST_F(GameObjectTestSchould, AddAndGetComponentByTemplete)
{
    CreateSut();

    sut_->AddComponent<RendererComponent>().AddModel(model);
    sut_->AddComponent<Animator>().AddAnimationClip(clip, "testFile");

    verifyComponents();
}

TEST_F(GameObjectTestSchould, AddByTreeNodeAndGetComponentByTemplete)
{
    CreateSut();

    Animator::registerReadFunctions();
    RendererComponent::registerReadFunctions();

    {
        TreeNode node("Component");
        node.attributes_["type"]              = "Animator";
        auto& animationClipsNode              = node.addChild("animationClips");
        auto& animationClipNode               = animationClipsNode.addChild("animationClip");
        animationClipNode.attributes_["name"] = clip;
        sut_->AddComponent(node);
    }

    {
        TreeNode node("Component");
        node.attributes_["type"] = "RendererComponent";
        auto& modelFileNamesNode = node.addChild("modelFileNames");
        {
            auto& modelFileNameNode = modelFileNamesNode.addChild("modelFileName");
            modelFileNameNode.addChild("fileName", std::string(model));
            modelFileNameNode.addChild("lvlOfDetail", std::string("0"));
        }
        sut_->AddComponent(node);
    }

    verifyComponents();

    unregsiterComponentReadFunction("Animator");
    unregsiterComponentReadFunction("RendererComponent");

    EXPECT_TRUE(ReadFunctions::instance().getComponentTypeNameToId().empty());
}

TEST_F(GameObjectTestSchould, CreateDeleteGameObjectStability)
{
    CreateSut();
    for (int i = 0; i < 2; ++i)
    {
        auto go = scene->CreateGameObject("TestGameObjectName_" + std::to_string(i));
        go.reset();
    }

    // Remove root
    scene->RemoveGameObject(0);
    scene->ProcessEvents();

    auto& componentController = scene->getComponentController();
    EXPECT_TRUE(componentController.getComponentFunctions().empty());
    EXPECT_TRUE(componentController.getComponentsContainer().empty());
}

// TO DO : fix me
TEST_F(GameObjectTestSchould, DISABLED_AddComponentToAwaknedObject)
{
    CreateSut();

    auto& test = sut_->AddComponent<TestComponent>();
    CheckComponentRegistration<TestComponent>();

    EXPECT_TRUE(test.onAwakendCalled);
    EXPECT_FALSE(test.onStartCalled);
}