#pragma once
#include <gmock/gmock.h>
#include "GraphicsApi/WindowApi.hpp"

namespace GraphicsApi
{
class WindowApiMock : public IWindowApi
{
public:
    MOCK_METHOD0(Init, void());
    MOCK_METHOD4(CreateWindow, void(const std::string&, uint32, uint32, WindowType));
    MOCK_METHOD0(CreateContext, void());
    MOCK_METHOD0(DeleteContext, void());
    MOCK_METHOD0(ProcessEvents, void());
    MOCK_METHOD0(UpdateWindow, void());
    MOCK_METHOD1(SetFullScreen, void(bool));
    MOCK_METHOD0(CheckActiveWindow, bool());
    MOCK_METHOD1(ShowCursor, void(bool));
    MOCK_METHOD0(CreateInput, std::shared_ptr<Input::InputManager>());
    MOCK_METHOD0(GetTime, double());
    MOCK_METHOD2(SetCursorPosition, void(int x, int y));
    MOCK_METHOD2(OpenFont, std::optional<uint32>(const std::string&, uint32));
    MOCK_METHOD4(RenderFont, std::optional<Surface>(uint32 id, const std::string&, const vec4&, uint32));
    MOCK_METHOD1(DeleteSurface, void(uint32));
    MOCK_METHOD0(BeginFrame, void());
};
}  // namespace GraphicsApi