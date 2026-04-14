#include <GameEngine/Renderers/GUI/Button/Button.h>
#include <GameEngine/Renderers/GUI/EditText/EditText.h>
#include <GameEngine/Renderers/GUI/Layout/HorizontalLayout.h>
#include <GameEngine/Renderers/GUI/Layout/VerticalLayout.h>
#include <GameEngine/Renderers/GUI/Manager.h>
#include <GameEngine/Renderers/GUI/Text/Text.h>
#include <GameEngine/Renderers/GUI/Window/Window.h>
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
    : sut(inputManager)
    {
    }

    NiceMock<Input::InputManagerMock> inputManager;
    GUI::Manager sut;
};

TEST_F(GuiManagerTests, addRemoveButton)
{
    auto &layer = sut.createLayer("1");

    std::atomic_bool isRunning(true);
    std::thread updateThread(
        [&]()
        {
            while (isRunning)
            {
                sut.update(0.1f);
                std::this_thread::sleep_for(std::chrono::microseconds(static_cast<int>(getRandomFloat() * 100)));
            }
        });

    std::vector<std::string> callLog;
    for (int i = 0; i < 100; i++)
    {
        auto window = std::make_unique<GUI::Window>(GUI::WindowStyle::FULL);
        auto button = std::make_unique<GUI::Button>();

        window->addChild(std::move(button));
        layer.add(std::move(window));
        std::this_thread::sleep_for(std::chrono::microseconds(static_cast<int>(getRandomFloat() * 100)));
    }

    isRunning.store(false);
    updateThread.join();

    LOG_DEBUG << "TestDone";
    SUCCEED();
}
