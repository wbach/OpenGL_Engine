#include <GameEngine/Components/ComponentController.h>
#include <GameEngine/Objects/GameObject.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

#include "Components/ComponentType.h"
#include "Logger/Log.h"
#include "Types.h"

using namespace GameEngine::Components;

namespace
{
// -----------------------------------------------------------------------------
// Stale ComponentType
// -----------------------------------------------------------------------------
const ComponentType TRANSFORM{.id = 1, .name = "Transform"};
const ComponentType RENDERER{.id = 2, .name = "Renderer"};
const ComponentType ANIMATOR{.id = 3, .name = "Animator"};
const ComponentType BOWPOSEUPDATER{.id = 4, .name = "BowPoseUpdater"};

struct Component : public IComponent
{
    Component(ComponentType type)
        : type{type}
    {
    }
    void CleanUp() override
    {
    }
    void Reload() override
    {
    }
    void ReqisterFunctions() override
    {
    }
    bool IsActive() const override
    {
        return true;
    }
    void Activate() override
    {
    }
    void SetActive(bool) override
    {
    }
    void Deactivate() override
    {
    }
    GameEngine::GameObject& GetParentGameObject() override
    {
        LOG_ERROR << "Should not be used here";
        return *empty;
    }
    const GameEngine::GameObject& getParentGameObject() const override
    {
        LOG_ERROR << "Should not be used here";
        return *empty;
    }
    void write(TreeNode&) const override
    {
    }
    std::optional<IdType> getRegisteredFunctionId(FunctionType) const override
    {
        return {};
    }
    std::vector<FieldInfo> GetFields() override
    {
        return {};
    }
    ComponentTypeID GetTypeId() const override
    {
        return type.id;
    }
    const std::string& GetTypeName() const override
    {
        return type.name;
    }

private:
    std::unique_ptr<GameEngine::GameObject> empty;
    ComponentType type;
};

}  // namespace

// -----------------------------------------------------------------------------
// TEST: Zaleznosci respektowane w kolejnosci wywolan
// -----------------------------------------------------------------------------
TEST(ComponentControllerTest, FunctionExecutionOrder_RespectsDependencies)
{
    ComponentController controller;
    std::vector<std::string> callLog;
    IdType gameObjectId = 1;

    controller.RegisterFunction(gameObjectId, TRANSFORM, FunctionType::Update,
                                [&callLog]() { callLog.push_back(TRANSFORM.name); }, {});

    controller.RegisterFunction(gameObjectId, RENDERER, FunctionType::Update, [&callLog]() { callLog.push_back(RENDERER.name); },
                                {TRANSFORM});

    controller.RegisterFunction(gameObjectId, ANIMATOR, FunctionType::Update, [&callLog]() { callLog.push_back(ANIMATOR.name); },
                                {RENDERER});

    controller.RegisterFunction(gameObjectId, NULL_COMPONENT_TYPE, FunctionType::Update,
                                [&callLog]() { callLog.push_back(NULL_COMPONENT_TYPE.name); }, {});

    controller.OnObjectCreated(gameObjectId);
    controller.CallFunctions(FunctionType::Update);

    ASSERT_EQ(callLog.size(), 4u);
    EXPECT_EQ(callLog[0], TRANSFORM.name);
    EXPECT_EQ(callLog[1], RENDERER.name);
    EXPECT_EQ(callLog[2], ANIMATOR.name);
    EXPECT_EQ(callLog[3], NULL_COMPONENT_TYPE.name);
}

// -----------------------------------------------------------------------------
// TEST: Losowa rejestracja, sprawdzenie sortowania zaleznosci
// -----------------------------------------------------------------------------
TEST(ComponentControllerTest, FunctionExecutionOrder_RespectsDependencies_RandomRegistration)
{
    ComponentController controller;
    std::vector<std::string> callLog;
    IdType gameObjectId = 1;

    // Rejestracja w losowej kolejnosci
    controller.RegisterFunction(gameObjectId, ANIMATOR, FunctionType::Update, [&callLog]() { callLog.push_back(ANIMATOR.name); },
                                {RENDERER});

    controller.RegisterFunction(gameObjectId, NULL_COMPONENT_TYPE, FunctionType::Update,
                                [&callLog]() { callLog.push_back(NULL_COMPONENT_TYPE.name); }, {});

    controller.RegisterFunction(gameObjectId, TRANSFORM, FunctionType::Update,
                                [&callLog]() { callLog.push_back(TRANSFORM.name); }, {});

    controller.RegisterFunction(gameObjectId, RENDERER, FunctionType::Update, [&callLog]() { callLog.push_back(RENDERER.name); },
                                {TRANSFORM});

    controller.OnObjectCreated(gameObjectId);
    controller.CallFunctions(FunctionType::Update);

    ASSERT_EQ(callLog.size(), 4u);
    EXPECT_EQ(callLog[0], TRANSFORM.name);
    EXPECT_EQ(callLog[1], RENDERER.name);
    EXPECT_EQ(callLog[2], ANIMATOR.name);
    EXPECT_EQ(callLog[3], NULL_COMPONENT_TYPE.name);
}

// -----------------------------------------------------------------------------
// TEST: Brak wymaganej zaleznosci
// -----------------------------------------------------------------------------
TEST(ComponentControllerTest, FunctionExecutionOrder_MissingDependency)
{
    ComponentController controller;
    std::vector<std::string> callLog;
    IdType gameObjectId = 1;

    controller.RegisterFunction(gameObjectId, ANIMATOR, FunctionType::Update, [&callLog]() { callLog.push_back(ANIMATOR.name); },
                                {RENDERER});  // Renderer nie istnieje

    controller.RegisterFunction(gameObjectId, NULL_COMPONENT_TYPE, FunctionType::Update,
                                [&callLog]() { callLog.push_back(NULL_COMPONENT_TYPE.name); }, {});

    controller.RegisterFunction(gameObjectId, TRANSFORM, FunctionType::Update,
                                [&callLog]() { callLog.push_back(TRANSFORM.name); }, {});

    controller.OnObjectCreated(gameObjectId);
    controller.CallFunctions(FunctionType::Update);

    // Funkcja Animator powinna zostac pominieta
    ASSERT_EQ(callLog.size(), 2u);
    EXPECT_EQ(callLog[0], TRANSFORM.name);
    EXPECT_EQ(callLog[1], NULL_COMPONENT_TYPE.name);
}

// -----------------------------------------------------------------------------
// TEST: Wykrywanie cyklicznych zaleznosci
// -----------------------------------------------------------------------------
TEST(ComponentControllerTest, FunctionExecutionOrder_CycleDependency)
{
    ComponentController controller;
    std::vector<std::string> callLog;
    IdType gameObjectId = 1;

    controller.RegisterFunction(gameObjectId, ANIMATOR, FunctionType::Update, [&callLog]() { callLog.push_back(ANIMATOR.name); },
                                {RENDERER});

    controller.RegisterFunction(gameObjectId, RENDERER, FunctionType::Update, [&callLog]() { callLog.push_back(RENDERER.name); },
                                {ANIMATOR});  // cykl

    controller.RegisterFunction(gameObjectId, NULL_COMPONENT_TYPE, FunctionType::Update,
                                [&callLog]() { callLog.push_back(NULL_COMPONENT_TYPE.name); }, {});

    EXPECT_THROW(controller.OnObjectCreated(gameObjectId), std::runtime_error);
}

// -----------------------------------------------------------------------------
// TEST: Zaleznosc do wielu
// -----------------------------------------------------------------------------
TEST(ComponentControllerTest, FunctionExecutionOrder_MultipleDependency)
{
    ComponentController controller;
    std::vector<std::string> callLog;
    IdType gameObjectId = 1;

    controller.RegisterFunction(gameObjectId, BOWPOSEUPDATER, FunctionType::Update,
                                [&callLog]() { callLog.push_back(BOWPOSEUPDATER.name); }, {ANIMATOR, TRANSFORM});

    controller.RegisterFunction(gameObjectId, ANIMATOR, FunctionType::Update, [&callLog]() { callLog.push_back(ANIMATOR.name); },
                                {RENDERER});

    controller.RegisterFunction(gameObjectId, RENDERER, FunctionType::Update, [&callLog]() { callLog.push_back(RENDERER.name); },
                                {TRANSFORM});

    controller.RegisterFunction(gameObjectId, TRANSFORM, FunctionType::Update,
                                [&callLog]() { callLog.push_back(TRANSFORM.name); }, {});

    controller.RegisterFunction(gameObjectId, NULL_COMPONENT_TYPE, FunctionType::Update,
                                [&callLog]() { callLog.push_back(NULL_COMPONENT_TYPE.name); }, {});

    controller.OnObjectCreated(gameObjectId);
    controller.CallFunctions(FunctionType::Update);

    ASSERT_EQ(callLog.size(), 5u);
    EXPECT_EQ(callLog[0], TRANSFORM.name);
    EXPECT_EQ(callLog[1], RENDERER.name);
    EXPECT_EQ(callLog[2], ANIMATOR.name);
    EXPECT_EQ(callLog[3], BOWPOSEUPDATER.name);
    EXPECT_EQ(callLog[4], NULL_COMPONENT_TYPE.name);
}

TEST(ComponentControllerTest, GetAllComponentsOfType_ReturnsOnlyRequestedType)
{
    struct RendererComponentTest : public Component
    {
        RendererComponentTest(IdType id)
            : Component{GetComponentType<RendererComponentTest>()}
            , id{id}
        {
        }

        IdType GetId()
        {
            return id;
        }

        IdType id;
    };

    struct RigidbodyComponentTest : public Component
    {
        RigidbodyComponentTest(IdType id)
            : Component{GetComponentType<RigidbodyComponentTest>()}
            , id{id}
        {
        }

        IdType GetId()
        {
            return id;
        }

        IdType id;
    };

    ComponentController controller;
    auto renderer1 = std::make_unique<RendererComponentTest>(1);
    auto renderer2 = std::make_unique<RendererComponentTest>(2);
    auto rigidbody = std::make_unique<RigidbodyComponentTest>(3);

    controller.RegisterComponent(renderer1->GetTypeId(), renderer1.get());
    controller.RegisterComponent(renderer2->GetTypeId(), renderer2.get());
    controller.RegisterComponent(rigidbody->GetTypeId(), rigidbody.get());

    auto renderers = controller.GetAllComponentsOfType<RendererComponentTest>();

    ASSERT_EQ(renderers.size(), 2u);
    std::vector<IdType> rendererIds;
    for (auto* r : renderers)
        rendererIds.push_back(r->GetId());
    EXPECT_THAT(rendererIds, ::testing::UnorderedElementsAre(1, 2));

    auto rigidbodies = controller.GetAllComponentsOfType<RigidbodyComponentTest>();
    ASSERT_EQ(rigidbodies.size(), 1u);
    EXPECT_EQ(rigidbodies[0]->GetId(), 3);

    auto missing = controller.GetAllComponentsOfType<IComponent>();
    EXPECT_TRUE(missing.empty());
}
