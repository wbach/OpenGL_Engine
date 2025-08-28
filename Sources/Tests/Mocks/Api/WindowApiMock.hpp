#pragma once
#include <gmock/gmock.h>

#include "GraphicsApi/WindowApi.hpp"
#include "Input/InputManager.h"

namespace GraphicsApi
{
class WindowApiMock : public IWindowApi
{
public:
    MOCK_METHOD0(Init, void());
    MOCK_METHOD4(CreateGameWindow, void(const std::string &, uint32, uint32, WindowType));
    MOCK_METHOD1(SetWindowSize, void(const vec2ui &));
    MOCK_CONST_METHOD0(GetWindowSize, vec2ui());
    MOCK_METHOD0(CreateContext, void());
    MOCK_METHOD0(DeleteContext, void());
    MOCK_METHOD0(ProcessEvents, void());
    MOCK_METHOD0(UpdateWindow, void());
    MOCK_METHOD1(SubscribeForEvent, IdType(std::function<void(const GraphicsApi::IWindowApi::Event &)>));
    MOCK_METHOD1(UnsubscribeForEvent, void(IdType));
    MOCK_METHOD1(SetFullScreen, void(bool));
    MOCK_METHOD0(CheckActiveWindow, bool());
    MOCK_METHOD1(ShowCursor, void(bool));
    MOCK_METHOD0(GetInputManager, Input::InputManager &());
    MOCK_METHOD0(GetTime, double());
    MOCK_METHOD2(SetCursorPosition, void(int x, int y));
    MOCK_METHOD0(BeginFrame, void());
    MOCK_CONST_METHOD2(ShowMessageBox, void(const std::string &, const std::string &));
    MOCK_CONST_METHOD3(ShowMessageBox, void(const std::string &, const std::string &, std::function<void(bool)>));
    MOCK_CONST_METHOD0(GetDisplayModes, const std::vector<DisplayMode> &());
};
}  // namespace GraphicsApi
