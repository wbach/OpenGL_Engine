#include "WxOpenGLApiWrapper.h"

#include <Logger/Log.h>

#include <GraphicsApi/WindowApi.hpp>
#include <memory>
#include <vector>
#include "WxInputManager.h"

namespace WxEditor
{
class WxWindowApi : public GraphicsApi::IWindowApi
{
public:
    WxWindowApi(const vec2i renderSize)
    {
        DEBUG_LOG("Add display mode: " + std::to_string(renderSize));
        displayModes.push_back(
            GraphicsApi::DisplayMode{.w = renderSize.x, .h = renderSize.y, .refreshRate = 60, .displayIndex = 0});
    }
    ~WxWindowApi() override;

    void Init() override;
    void CreateGameWindow(const std::string &, uint32, uint32, GraphicsApi::WindowType) override;
    void SetWindowSize(const vec2ui &) override;
    vec2ui GetWindowSize() const override;
    void CreateContext() override;
    void DeleteContext() override;
    void ProcessEvents() override;
    void UpdateWindow() override;
    IdType SubscribeForEvent(std::function<void(const GraphicsApi::IWindowApi::Event &)>) override;
    void UnsubscribeForEvent(IdType) override;
    void SetFullScreen(bool full_screen) override;
    bool CheckActiveWindow() override;
    void ShowCursor(bool show) override;
    std::unique_ptr<Input::InputManager> CreateInput() override;
    double GetTime() override;
    void SetCursorPosition(int x, int y) override;
    void ShowMessageBox(const std::string &title, const std::string &message) const override;
    void ShowMessageBox(const std::string &title, const std::string &message, std::function<void(bool)>) const override;
    const std::vector<GraphicsApi::DisplayMode> &GetDisplayModes() const override;
    void BeginFrame() override;

private:
    std::vector<GraphicsApi::DisplayMode> displayModes;
};

WxWindowApi::~WxWindowApi()
{
}

void WxWindowApi::Init()
{
}

void WxWindowApi::CreateGameWindow(const std::string &, uint32, uint32, GraphicsApi::WindowType)
{
    DEBUG_LOG("CreateGameWindow not needed");
}

void WxWindowApi::SetWindowSize(const vec2ui &)
{
}

vec2ui WxWindowApi::GetWindowSize() const
{
    return {};
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

std::unique_ptr<Input::InputManager> WxWindowApi::CreateInput()
{
    return std::make_unique<WxInputManager>();
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

WxOpenGLApiWrapper::WxOpenGLApiWrapper(const vec2i renderSize)
    : OpenGLApi::OpenGLApi(std::make_unique<WxWindowApi>(renderSize))
{
    SetBackgroundColor(Color(0.18f, 0.27f, 0.47f));
}

void WxOpenGLApiWrapper::CreateContext()
{
    DEBUG_LOG("Skip createContext, should already be created");
}

}  // namespace WxEditor
