#include <GameEngine/Components/ComponentController.h>
#include <gtest/gtest.h>

#include "Components/ComponentType.h"
#include "Types.h"

using namespace GameEngine::Components;

// -----------------------------------------------------------------------------
// Stale ComponentType
// -----------------------------------------------------------------------------
const ComponentType TRANSFORM{.id = 1, .name = "Transform"};
const ComponentType RENDERER{.id = 2, .name = "Renderer"};
const ComponentType ANIMATOR{.id = 3, .name = "Animator"};

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
