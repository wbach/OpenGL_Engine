#pragma once
#include <GraphicsApi/WindowApi.hpp>
#include <vector>

#include "Types.h"
#include "WxInputManager.h"

class wxMouseEvent;

namespace WxEditor
{
class WxWindowApi : public GraphicsApi::IWindowApi
{
public:
    WxWindowApi(const vec2i&, WxInputManager::WrapPointerFunc);
    ~WxWindowApi() override;

    void OnMouseMove(wxMouseEvent &);

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
    Input::InputManager &GetInputManager() override;
    double GetTime() override;
    void SetCursorPosition(int x, int y) override;
    void ShowMessageBox(const std::string &title, const std::string &message) const override;
    void ShowMessageBox(const std::string &title, const std::string &message, std::function<void(bool)>) const override;
    const std::vector<GraphicsApi::DisplayMode> &GetDisplayModes() const override;
    void BeginFrame() override;
    WxInputManager &GetWxInputManager()
    {
        return *inputManager;
    }

private:
    std::vector<GraphicsApi::DisplayMode> displayModes;
    std::unique_ptr<WxInputManager> inputManager;
    vec2ui windowSize;
    IdType windowSizeSub;
};
}  // namespace WxEditor
