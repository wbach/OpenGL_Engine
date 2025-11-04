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
        RegisterFunction(FunctionType::LateAwake, [this]() { onLateAwakeCalled = true; });
        RegisterFunction(FunctionType::OnStart, [this]() { onStartCalled = true; });
        RegisterFunction(FunctionType::PostStart, [this]() { onPostStartCalled = true; });
        RegisterFunction(FunctionType::Update, [this]() { onUpdateCalled = true; });
        RegisterFunction(FunctionType::PostUpdate, [this]() { onPostUpdateCalled = true; });
        RegisterFunction(FunctionType::AlwaysUpdate, [this]() { onAlwaysUpdateCalled = true; });
    }
    void CleanUp() override
    {
        cleanUpCalled = true;
    }
    void Reload() override
    {
    }

    bool cleanUpCalled{false};
    bool onAwakendCalled{false};
    bool onLateAwakeCalled{false};
    bool onStartCalled{false};
    bool onPostStartCalled{false};
    bool onUpdateCalled{false};
    bool onPostUpdateCalled{false};
    bool onAlwaysUpdateCalled{false};
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

TEST_F(GameObjectTestSchould, AddComponentToAwaknedObject)
{
    CreateSut();

    auto& test = sut_->AddComponent<TestComponent>();
    CheckComponentRegistration<TestComponent>();

    EXPECT_TRUE(test.onAwakendCalled);
    EXPECT_TRUE(test.onLateAwakeCalled);
    EXPECT_FALSE(test.onStartCalled);
    EXPECT_FALSE(test.onPostStartCalled);
}

TEST_F(GameObjectTestSchould, AddComponentToStartedObject)
{
    CreateSut();
    scene->Start();

    auto& test = sut_->AddComponent<TestComponent>();
    CheckComponentRegistration<TestComponent>();

    EXPECT_TRUE(test.onAwakendCalled);
    EXPECT_TRUE(test.onStartCalled);
    EXPECT_TRUE(test.onPostStartCalled);
    EXPECT_TRUE(test.onLateAwakeCalled);
}

TEST_F(GameObjectTestSchould, AddObjectToScene)
{
    auto go    = scene->CreateGameObject("SUT");
    sut_       = go.get();
    auto& test = sut_->AddComponent<TestComponent>();

    // No called when object not added to scene
    EXPECT_FALSE(test.onAwakendCalled);
    EXPECT_FALSE(test.onLateAwakeCalled);
    EXPECT_FALSE(test.onStartCalled);
    EXPECT_FALSE(test.onPostStartCalled);

    scene->AddGameObject(std::move(go));

    // No called when addition event wasnt processed
    EXPECT_FALSE(test.onAwakendCalled);
    EXPECT_FALSE(test.onStartCalled);

    scene->ProcessEvents();

    // Scene is not started. So after event only awake should be called
    EXPECT_TRUE(test.onAwakendCalled);
    EXPECT_TRUE(test.onLateAwakeCalled);
    EXPECT_FALSE(test.onStartCalled);
    EXPECT_FALSE(test.onPostStartCalled);

    scene->FullUpdate(0.1f);
    scene->PostUpdate();

    EXPECT_FALSE(test.onUpdateCalled);
    EXPECT_FALSE(test.onPostUpdateCalled);
    EXPECT_TRUE(test.onAlwaysUpdateCalled);
}

TEST_F(GameObjectTestSchould, AddObjectToStartedScene)
{
    scene->Start();

    auto go    = scene->CreateGameObject("SUT");
    sut_       = go.get();
    auto& test = sut_->AddComponent<TestComponent>();

    // No called when object not added to scene
    EXPECT_FALSE(test.onAwakendCalled);
    EXPECT_FALSE(test.onStartCalled);

    scene->AddGameObject(std::move(go));

    // No called when addition event wasnt processed
    EXPECT_FALSE(test.onAwakendCalled);
    EXPECT_FALSE(test.onStartCalled);

    scene->ProcessEvents();

    // Scene is started. So after event should be called
    EXPECT_TRUE(test.onAwakendCalled);
    EXPECT_TRUE(test.onLateAwakeCalled);
    EXPECT_TRUE(test.onStartCalled);
    EXPECT_TRUE(test.onPostStartCalled);

    EXPECT_FALSE(test.onUpdateCalled);
    EXPECT_FALSE(test.onPostUpdateCalled);

    scene->FullUpdate(0.1f);
    scene->PostUpdate();

    EXPECT_TRUE(test.onUpdateCalled);
    EXPECT_TRUE(test.onPostUpdateCalled);
    EXPECT_TRUE(test.onAlwaysUpdateCalled);
}

TEST_F(GameObjectTestSchould, CleanUpDuingRemovalByType)
{
    CreateSut();
    scene->Start();

    auto& test = sut_->AddComponent<TestComponent>();
    sut_->RemoveComponent<TestComponent>();

    EXPECT_TRUE(test.cleanUpCalled);
}

TEST_F(GameObjectTestSchould, CleanUpDuingRemovalByPtr)
{
    CreateSut();
    scene->Start();

    auto& test = sut_->AddComponent<TestComponent>();
    sut_->RemoveComponent(test);

    EXPECT_TRUE(test.cleanUpCalled);
}

TEST_F(GameObjectTestSchould, CleanUpDuingRemovalByTyeId)
{
    CreateSut();
    scene->Start();

    auto& test = sut_->AddComponent<TestComponent>();
    sut_->RemoveComponent(test.GetTypeId());

    EXPECT_TRUE(test.cleanUpCalled);
}

TEST_F(GameObjectTestSchould, CleanUpDuingAllComponentsRemoval)
{
    CreateSut();
    scene->Start();
    auto& test = sut_->AddComponent<TestComponent>();
    sut_->RemoveAllComponents();

    EXPECT_TRUE(test.cleanUpCalled);
}

TEST_F(GameObjectTestSchould, CleanUpDuingMultipleTheSameComponentsAllRemoval)
{
    CreateSut();
    scene->Start();
    auto& test  = sut_->AddComponent<TestComponent>();
    auto& test2 = sut_->AddComponent<TestComponent>();
    auto& test3 = sut_->AddComponent<TestComponent>();

    sut_->RemoveAllComponents();

    EXPECT_TRUE(test.cleanUpCalled);
    EXPECT_TRUE(test2.cleanUpCalled);
    EXPECT_TRUE(test3.cleanUpCalled);
}

TEST_F(GameObjectTestSchould, CleanUpDuingMultipleTheSameComponentsRemoval)
{
    CreateSut();
    scene->Start();
    auto& test  = sut_->AddComponent<TestComponent>();
    auto& test2 = sut_->AddComponent<TestComponent>();
    auto& test3 = sut_->AddComponent<TestComponent>();

    EXPECT_FALSE(test.cleanUpCalled);
    EXPECT_FALSE(test2.cleanUpCalled);
    EXPECT_FALSE(test3.cleanUpCalled);

    sut_->RemoveComponent(test);
    EXPECT_TRUE(test.cleanUpCalled);
    EXPECT_FALSE(test2.cleanUpCalled);
    EXPECT_FALSE(test3.cleanUpCalled);

    sut_->RemoveComponent(test2);

    EXPECT_TRUE(test.cleanUpCalled);
    EXPECT_TRUE(test2.cleanUpCalled);
    EXPECT_FALSE(test3.cleanUpCalled);
    sut_->RemoveComponent(test3);

    EXPECT_TRUE(test.cleanUpCalled);
    EXPECT_TRUE(test2.cleanUpCalled);
    EXPECT_TRUE(test3.cleanUpCalled);

    EXPECT_TRUE(sut_->GetComponents().empty());
}