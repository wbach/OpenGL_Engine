#pragma once
#include <Types.h>

#include <GraphicsApi/WindowApi.hpp>
#include <functional>
#include <memory>


#undef CreateWindow
namespace GraphicsApi::Dx11
{
struct DirectXContext;

class WinApi : public IWindowApi
{
public:
    WinApi(DirectXContext&);
    ~WinApi() override;
    void Init() override;
    void CreateGameWindow(const std::string& window_name, uint32 width, uint32 height, WindowType full_screen) override;
    void SetWindowSize(const vec2ui&) override;
    vec2ui GetWindowSize() const override;

    void CreateContext() override;
    void DeleteContext() override;

    void ProcessEvents() override;
    void UpdateWindow() override;
    IdType SubscribeForEvent(std::function<void(const IWindowApi::Event&)>) override;
    void UnsubscribeForEvent(IdType) override;
    void SetFullScreen(bool full_screen) override;
    bool CheckActiveWindow() override;

    void BeginFrame() override;
    void ShowCursor(bool show) override;
    Input::InputManager& GetInputManager() override;
    double GetTime() override;
    void SetCursorPosition(int x, int y) override;

    void ShowMessageBox(const std::string&, const std::string&) const override;
    void ShowMessageBox(const std::string&, const std::string&, std::function<void(bool)>) const override;

    const std::vector<DisplayMode>& GetDisplayModes() const override;

private:
    class Pimpl;
    std::unique_ptr<Pimpl> impl_;
};
}  // namespace GraphicsApi::Dx11
