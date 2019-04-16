#pragma once
#include <functional>
#include <memory>
#include "GraphicsApi/WindowApi.hpp"
#include "Types.h"

#undef CreateWindow

namespace DirectX
{
struct DirectXContext;

class WinApi : public GraphicsApi::IWindowApi
{
public:
    WinApi(DirectXContext& directXContext);
    virtual ~WinApi() override;
    virtual void Init() override;
    virtual void CreateWindow(const std::string& window_name, uint32 width, uint32 height,
                              GraphicsApi::WindowType full_screen) override;
    virtual void CreateContext() override;
    virtual void DeleteContext() override;

    virtual void ProcessEvents() override;
    virtual void UpdateWindow() override;
    virtual void SetFullScreen(bool full_screen) override;
    virtual bool CheckActiveWindow() override;

    virtual void BeginFrame() override;
    virtual void ShowCursor(bool show) override;
    virtual std::shared_ptr<Input::InputManager> CreateInput() override;
    virtual double GetTime() override;
    virtual void SetCursorPosition(int x, int y) override;

    virtual std::optional<uint32> OpenFont(const std::string& filename, uint32 size) override;
    virtual std::optional<GraphicsApi::Surface> RenderFont(uint32 id, const std::string& text, const vec4& color,
                                                           uint32 outline) override;
    virtual void DeleteSurface(uint32 surfaceId) override;

private:
    uint32 CreateWindowFlags(GraphicsApi::WindowType type) const;
    void CreateSDLWindow(const std::string& window_name, const int& width, const int& height, uint32 flags);

private:
    bool RegiesterWindowClass();
    void MessageLoop();
    void ProcessSdlEvent() const;
    void ProccesSdlKeyDown(uint32 type) const;

private:
    struct Pimpl;
    std::unique_ptr<Pimpl> impl_;
    uint32 startTime;
    bool fullScreenActive;

    std::function<void(uint32, uint32)> addKeyEvent_;
};
}  // namespace DirectX
