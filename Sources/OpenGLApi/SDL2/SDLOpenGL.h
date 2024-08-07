#pragma once
#include <Utils/IdPool.h>
#include <functional>
#include <memory>
#include "GraphicsApi/WindowApi.hpp"
#include "Mutex.hpp"
#include "Types.h"

namespace OpenGLApi
{
class SdlOpenGlApi : public GraphicsApi::IWindowApi
{
public:

    SdlOpenGlApi();
    ~SdlOpenGlApi() override;
    void Init() override;
    void CreateGameWindow(const std::string& window_name, uint32 width, uint32 height,
                          GraphicsApi::WindowType full_screen) override;
    void SetWindowSize(const vec2ui&) override;
    vec2ui GetWindowSize() const override;
    void CreateContext() override;
    void DeleteContext() override;

    void ProcessEvents() override;
    void UpdateWindow() override;
    IdType SubscribeForEvent(std::function<void(const GraphicsApi::IWindowApi::Event&)>) override;
    void UnsubscribeForEvent(IdType) override;

    void SetFullScreen(bool full_screen) override;
    bool CheckActiveWindow() override;

    void BeginFrame() override;
    void ShowCursor(bool show) override;
    std::unique_ptr<Input::InputManager> CreateInput() override;
    double GetTime() override;
    void SetCursorPosition(int x, int y) override;

    void ShowMessageBox(const std::string&, const std::string&) const override;
    void ShowMessageBox(const std::string&, const std::string&, std::function<void(bool)>) const override;

    const std::vector<GraphicsApi::DisplayMode>& GetDisplayModes() const override;

private:
    void FillDisplayModes();
    uint32 CreateWindowFlags(GraphicsApi::WindowType type) const;
    void CreateSDLWindow(const std::string& window_name, const int& width, const int& height, uint32 flags);

private:
    void ProcessSdlEvent();
    void ProccesSdlKeyDown(uint32 type) const;

private:
    struct Pimpl;
    std::unique_ptr<Pimpl> impl_;
    uint32 startTime;
    vec2ui windowSize_;

    std::function<void(uint32, uint32)> addKeyEvent_;
    std::vector<GraphicsApi::DisplayMode> displayModes_;

    Utils::IdPool eventSubscribersEventsPool_;
    std::mutex eventSubscribersMutex_;
    std::unordered_map<IdType, std::function<void(const GraphicsApi::IWindowApi::Event&)>> eventsSubscribers_;
};
}  // namespace OpenGLApi
