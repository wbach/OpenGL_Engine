#pragma once
#include <gmock/gmock.h>
#include "GraphicsApi/WindowApi.hpp"

namespace GraphicsApi
{
class WindowApiMock : public IWindowApi
{
public:
    MOCK_METHOD4(CreateWindow, void(const std::string& window_name, uint32 width, uint32 height, WindowType type));
    MOCK_METHOD0(CreateContext, void());
    MOCK_METHOD0(DeleteContext, void());
    MOCK_METHOD0(ProcessEvents, void());
    MOCK_METHOD0(UpdateWindow, void());
    MOCK_METHOD1(SetFullScreen, void(bool full_screen));
    MOCK_METHOD0(CheckActiveWindow, bool());
    MOCK_METHOD1(ShowCursor, void(bool show));
    MOCK_METHOD0(CreateInput, std::shared_ptr<Input::InputManager>());
    MOCK_METHOD0(GetTime, double());
    MOCK_METHOD2(SetCursorPosition, void(int x, int y));
    MOCK_METHOD2(OpenFont, uint32(const std::string& filename, uint32 size));
    MOCK_METHOD4(RenderFont, Surface(uint32 id, const std::string& text, const vec4& color, uint32 outline));
    MOCK_METHOD1(DeleteSurface, void(uint32 surfaceId));
    MOCK_METHOD0(BeginFrame, void());
};
}  // namespace GraphicsApi