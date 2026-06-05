#pragma once
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

#include "GraphicsApi/WindowApi.hpp"
#include "VulkanContext.hpp"

namespace VulkanApi
{
class SdlVulkanApi : public GraphicsApi::IWindowApi
{
public:
    SdlVulkanApi(GraphicsApi::VulkanContext&);
    ~SdlVulkanApi() override;

    void Init() override;
    void CreateGameWindow(const std::string& window_name, uint32 width, uint32 height,
                          GraphicsApi::WindowType windowType) override;
    void SetWindowSize(const vec2ui& size) override;
    vec2ui GetWindowSize() const override;

    void CreateContext() override;
    void DeleteContext() override;
    void UpdateWindow() override;

    IdType SubscribeForEvent(std::function<void(const GraphicsApi::IWindowApi::Event&)> f) override;
    void UnsubscribeForEvent(IdType id) override;
    void SetFullScreen(bool full_screen) override;
    bool CheckActiveWindow() override;
    void ShowCursor(bool show) override;
    Input::InputManager& GetInputManager() override;
    double GetTime() override;
    void SetCursorPosition(int x, int y) override;
    void ShowMessageBox(const std::string& title, const std::string& message) const override;
    void ShowMessageBox(const std::string& title, const std::string& msg, std::function<void(bool)> selectedFunc) const override;
    const std::vector<GraphicsApi::DisplayMode>& GetDisplayModes() const override;
    void ProcessEvents() override;

private:
    void FillDisplayModes();
    uint32 CreateWindowFlags(GraphicsApi::WindowType type) const;
    void CreateSDLWindow(const std::string& window_name, int width, int height, uint32 flags);
    void BeginFrame() override;
    void ProcessSdlEvent();
    void ProccesSdlKeyDown(uint32 type) const;

    void RecreateSwapChain();

private:
    GraphicsApi::VulkanContext& vkContext;
    struct Pimpl;
    std::unique_ptr<Pimpl> impl;

    std::unique_ptr<Input::InputManager> inputManager_;
    vec2ui windowSize_{0, 0};
    std::vector<GraphicsApi::DisplayMode> displayModes_;

    std::mutex eventSubscribersMutex_;
    std::unordered_map<IdType, std::function<void(const GraphicsApi::IWindowApi::Event&)>> eventsSubscribers_;
    uint32 startTime = 0;
};
}  // namespace VulkanApi
