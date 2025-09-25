#include <GameEngine/Components/ComponentController.h>
#include <gtest/gtest.h>

#include "Components/ComponentType.h"
#include "Types.h"

using namespace GameEngine::Components;

TEST(ComponentControllerTest, FunctionExecutionOrder_RespectsDependencies)
{
    ComponentController controller;
    std::vector<std::string> callLog;  // tu bedziemy logowac wywolania

    IdType gameObjectId = 1;

    // Zarejestrujmy funkcje komponentow z zaleznosciami
    controller.RegisterFunction(
        gameObjectId, ComponentTypeID{1}, FunctionType::Update, [&callLog]() { callLog.push_back("Transform"); },
        ComponentController::Dependencies{});

    controller.RegisterFunction(
        gameObjectId, ComponentTypeID{2}, FunctionType::Update, [&callLog]() { callLog.push_back("Renderer"); },
        ComponentController::Dependencies{1});

    controller.RegisterFunction(
        gameObjectId, ComponentTypeID{3}, FunctionType::Update, [&callLog]() { callLog.push_back("Animator"); },
        ComponentController::Dependencies{2});

    // Funkcja globalna (NULL_COMPONENT_ID), powinna byc na koncu
    controller.RegisterFunction(gameObjectId, NULL_COMPONENT_ID, FunctionType::Update,
                                [&callLog]() { callLog.push_back("Global"); }, {});

    controller.OnObjectCreated(gameObjectId);
    // Wywolujemy wszystkie funkcje Update
    controller.CallFunctions(FunctionType::Update);

    // Sprawdzenie kolejnosci
    ASSERT_EQ(callLog.size(), 4u);
    EXPECT_EQ(callLog[0], "Transform");
    EXPECT_EQ(callLog[1], "Renderer");
    EXPECT_EQ(callLog[2], "Animator");
    EXPECT_EQ(callLog[3], "Global");
}

TEST(ComponentControllerTest, FunctionExecutionOrder_RespectsDependencies_RandomRegistration)
{
    ComponentController controller;
    std::vector<std::string> callLog;  // tu bedziemy logowac wywolania

    IdType gameObjectId = 1;

    // Rejestracja funkcji w losowej kolejnosci
    controller.RegisterFunction(gameObjectId, 3, FunctionType::Update, [&callLog]() { callLog.push_back("Animator"); },
                                {2});  // Animator zalezy od Renderer

    controller.RegisterFunction(gameObjectId, NULL_COMPONENT_ID, FunctionType::Update,
                                [&callLog]() { callLog.push_back("Global"); }, {});  // globalna, powinna byc na koncu

    controller.RegisterFunction(gameObjectId, 1, FunctionType::Update, [&callLog]() { callLog.push_back("Transform"); },
                                {});  // Transform nie zalezy od nikogo

    controller.RegisterFunction(gameObjectId, 2, FunctionType::Update, [&callLog]() { callLog.push_back("Renderer"); },
                                {1});  // Renderer zalezy od Transform

    controller.OnObjectCreated(gameObjectId);
    // Wywolujemy wszystkie funkcje Update
    controller.CallFunctions(FunctionType::Update);

    // Sprawdzenie kolejnosci
    ASSERT_EQ(callLog.size(), 4u);
    EXPECT_EQ(callLog[0], "Transform");
    EXPECT_EQ(callLog[1], "Renderer");
    EXPECT_EQ(callLog[2], "Animator");
    EXPECT_EQ(callLog[3], "Global");
}
