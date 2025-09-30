#include <GameEngine/Renderers/GUI/Button/GuiButton.h>
#include <GameEngine/Renderers/GUI/EditText/GuiEditText.h>
#include <GameEngine/Renderers/GUI/GuiManager.h>
#include <GameEngine/Renderers/GUI/Layout/HorizontalLayout.h>
#include <GameEngine/Renderers/GUI/Layout/VerticalLayout.h>
#include <GameEngine/Renderers/GUI/Text/GuiTextElement.h>
#include <GameEngine/Renderers/GUI/Window/GuiWindow.h>
#include <gtest/gtest.h>

#include <atomic>
#include <memory>
#include <string>

#include "Logger/Log.h"
#include "Tests/Mocks/Api/InputManagerMock.h"
#include "gmock/gmock.h"

using namespace GameEngine;
using namespace ::testing;

struct GuiManagerTests : public ::testing::Test
{
    GuiManagerTests()
    {
        isOnTop = [](const auto &) { return true; };
    }

    std::function<bool(const GuiElement &)> isOnTop;
    NiceMock<Input::InputManagerMock> inputManager;
    GuiManager sut;
};

TEST_F(GuiManagerTests, addRemoveButton)
{
    sut.AddLayer("1");

    std::atomic_bool isRunning(true);
    std::thread updateThread(
        [&]()
        {
            while (isRunning)
            {
                sut.Update(0.1f);
                std::this_thread::sleep_for(std::chrono::microseconds(static_cast<int>(getRandomFloat() * 100)));
            }
        });

    std::vector<std::string> callLog;
    for (int i = 0; i < 100; i++)
    {
        OnClick onClik = [&](auto &) { callLog.push_back(std::to_string(i)); };
        auto window    = std::make_unique<GuiWindowElement>(GuiWindowStyle::FULL, inputManager);
        auto button    = std::make_unique<GuiButtonElement>(isOnTop, inputManager, onClik);

        window->AddChild(std::move(button));
        sut.Add("1", std::move(window));
        std::this_thread::sleep_for(std::chrono::microseconds(static_cast<int>(getRandomFloat() * 100)));
    }

    isRunning.store(false);
    updateThread.join();

    LOG_DEBUG << "TestDone";
    SUCCEED();
}
