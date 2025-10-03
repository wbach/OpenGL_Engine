#include "WxWindowApi.h"

#include <GameEngine/Engine/Configuration.h>
#include <Logger/Log.h>

namespace WxEditor
{
WxWindowApi::WxWindowApi(const vec2i renderSize, WxInputManager::WrapPointerFunc func)
    : inputManager{std::make_unique<WxInputManager>(renderSize, func)}
    , windowSize(renderSize.x, renderSize.y)
{
    LOG_DEBUG << "Add display mode: " << renderSize;
    displayModes.push_back(GraphicsApi::DisplayMode{.w = renderSize.x, .h = renderSize.y, .refreshRate = 60, .displayIndex = 0});

    windowSizeSub = EngineConf.window.size.subscribeForChange(
        [this]()
        {
            LOG_DEBUG << "Window size change detected.";
            windowSize = EngineConf.window.size;
        });
}

WxWindowApi::~WxWindowApi()
{
    EngineConf.window.size.unsubscribe(windowSizeSub);
}

void WxWindowApi::OnMouseMove(wxMouseEvent &event)
{
    inputManager->OnMouseMove(event);
}

void WxWindowApi::Init()
{
}

void WxWindowApi::CreateGameWindow(const std::string &, uint32, uint32, GraphicsApi::WindowType)
{
    LOG_DEBUG << "CreateGameWindow not needed";
}

void WxWindowApi::SetWindowSize(const vec2ui &)
{
}

vec2ui WxWindowApi::GetWindowSize() const
{
    return windowSize;
}

void WxWindowApi::CreateContext()
{
}

void WxWindowApi::DeleteContext()
{
}

void WxWindowApi::ProcessEvents()
{
}

void WxWindowApi::UpdateWindow()
{
}

IdType WxWindowApi::SubscribeForEvent(std::function<void(const Event &)>)
{
    return 0;
}

void WxWindowApi::UnsubscribeForEvent(IdType)
{
}

void WxWindowApi::SetFullScreen(bool full_screen)
{
}

bool WxWindowApi::CheckActiveWindow()
{
    return true;
}

void WxWindowApi::ShowCursor(bool show)
{
}

Input::InputManager &WxWindowApi::GetInputManager()
{
    assert(inputManager);
    return *inputManager;
}

double WxWindowApi::GetTime()
{
    return 0;
}

void WxWindowApi::SetCursorPosition(int x, int y)
{
}

void WxWindowApi::ShowMessageBox(const std::string &title, const std::string &message) const
{
}

void WxWindowApi::ShowMessageBox(const std::string &title, const std::string &message, std::function<void(bool)>) const
{
}

const std::vector<GraphicsApi::DisplayMode> &WxWindowApi::GetDisplayModes() const
{
    return displayModes;
}

void WxWindowApi::BeginFrame()
{
}
}  // namespace WxEditor
