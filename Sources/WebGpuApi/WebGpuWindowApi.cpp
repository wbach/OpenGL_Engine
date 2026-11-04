#include "WebGpuWindowApi.h"

#include <Logger/Log.h>

#include "InputWebGpu.h"

namespace GraphicsApi::WebGpu
{
WebGpuWindowApi::WebGpuWindowApi()
    : inputManager_(std::make_unique<InputWebGpu>())
{
}
WebGpuWindowApi::~WebGpuWindowApi()
{
}

void WebGpuWindowApi::Init()
{
}

void WebGpuWindowApi::CreateGameWindow(const std::string& window_name, uint32 width, uint32 height,
                                       GraphicsApi::WindowType)
{
    LOG_DEBUG << "Create Game Window " << window_name << " width: " << width << ", height:" << height;
    windowSize_ = vec2ui(width, height);
}

void WebGpuWindowApi::SetWindowSize(const vec2ui& size)
{
    windowSize_ = size;
}

vec2ui WebGpuWindowApi::GetWindowSize() const
{
    return windowSize_;
}

void WebGpuWindowApi::CreateContext()
{
}

void WebGpuWindowApi::DeleteContext()
{
}

void WebGpuWindowApi::ProcessEvents()
{
}

void WebGpuWindowApi::UpdateWindow()
{
}

IdType WebGpuWindowApi::SubscribeForEvent(std::function<void(const Event&)> callback)
{
    return {};
}

void WebGpuWindowApi::UnsubscribeForEvent(IdType id)
{
}

void WebGpuWindowApi::SetFullScreen(bool full_screen)
{
}

bool WebGpuWindowApi::CheckActiveWindow()
{
    return true;
}

void WebGpuWindowApi::ShowCursor(bool show)
{
}

Input::InputManager& WebGpuWindowApi::GetInputManager()
{
    return *inputManager_;
}

double WebGpuWindowApi::GetTime()
{
    return 0.0;
}

void WebGpuWindowApi::SetCursorPosition(int x, int y)
{
}

void WebGpuWindowApi::ShowMessageBox(const std::string& title, const std::string& message) const
{
}

void WebGpuWindowApi::ShowMessageBox(const std::string& title, const std::string& message,
                                     std::function<void(bool)> callback) const
{
}

const std::vector<DisplayMode>& WebGpuWindowApi::GetDisplayModes() const
{
    static const std::vector<DisplayMode> displayModes;
    return displayModes;
}

void WebGpuWindowApi::BeginFrame()
{
}

}  // namespace GraphicsApi::WebGpu
