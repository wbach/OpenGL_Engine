#include "WxOpenGLApiWrapper.h"

#include <Input/InputManager.h>
#include <Logger/Log.h>

#include <GraphicsApi/WindowApi.hpp>
#include <memory>
#include <vector>

namespace WxEditor
{
namespace
{
class WxInputManager : public Input::InputManager
{
public:
    bool GetKey(KeyCodes::Type)
    {
        return false;
    }
    bool GetMouseKey(KeyCodes::Type)
    {
        return false;
    }
    void SetReleativeMouseMode(bool)
    {
    }
    vec2i CalcualteMouseMove()
    {
        return {0, 0};
    }
    vec2i GetPixelMousePosition()
    {
        return {0, 0};
    }
    vec2 GetMousePosition()
    {
        return {0, 0};
    }
    void SetKeyToBuffer(int, bool)
    {
    }
    void ClearKeyBuffer()
    {
    }
    void SetCursorPosition(int, int)
    {
    }
    void GetPressedKeys()
    {
    }
    void ShowCursor(bool)
    {
    }

protected:
    KeyCodes::Type ConvertCode(uint32) const
    {
        return {};
    }
    bool IsKeyUpEventType(uint32) const
    {
        return false;
    }
    bool IsKeyDownEventType(uint32) const
    {
        return false;
    }
};
}  // namespace

class WxWindowApi : public GraphicsApi::IWindowApi
{
public:
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

WxOpenGLApiWrapper::WxOpenGLApiWrapper()
    : OpenGLApi::OpenGLApi(std::make_unique<WxWindowApi>())
{
    SetBackgroundColor(Color(0.18f, 0.27f, 0.47f));
}

void WxOpenGLApiWrapper::Init()
{
    DEBUG_LOG("Init");
    OpenGLApi::Init();
}

void WxOpenGLApiWrapper::CreateContext()
{
    DEBUG_LOG("Skip createContext, should already be created");
}


}  // namespace WxEditor
