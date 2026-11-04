#pragma once
#include "GraphicsApi/WindowApi.hpp"
#include "Types.h"

namespace GraphicsApi::WebGpu
{
class InputWebGpu;
class WebGpuWindowApi : public IWindowApi
{
public:
    WebGpuWindowApi();
    ~WebGpuWindowApi() override;

    void Init() override;

    void CreateGameWindow(const std::string& window_name, uint32 width, uint32 height,
                          GraphicsApi::WindowType windowType) override;

    void SetWindowSize(const vec2ui& size) override;
    vec2ui GetWindowSize() const override;

    void CreateContext() override;
    void DeleteContext() override;

    void ProcessEvents() override;
    void UpdateWindow() override;

    IdType SubscribeForEvent(std::function<void(const Event&)> callback) override;
    void UnsubscribeForEvent(IdType id) override;

    void SetFullScreen(bool full_screen) override;
    bool CheckActiveWindow() override;
    void ShowCursor(bool show) override;

    Input::InputManager& GetInputManager() override;
    double GetTime() override;
    void SetCursorPosition(int x, int y) override;

    void ShowMessageBox(const std::string& title, const std::string& message) const override;
    void ShowMessageBox(const std::string& title, const std::string& message, std::function<void(bool)> callback) const override;

    const std::vector<DisplayMode>& GetDisplayModes() const override;

    void BeginFrame() override;

private:
    vec2ui windowSize_{};
    std::unique_ptr<InputWebGpu> inputManager_;
};
}  // namespace GraphicsApi::WebGpu
