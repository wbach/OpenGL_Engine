#pragma once
#include <memory>
#include "ApiMessages.h"
#include "Surface.h"
#include "Types.h"
#include "WindowType.h"

namespace Input
{
class InputManager;
}  // namespace Input

namespace GraphicsApi
{
class IWindowApi
{
public:
    virtual ~IWindowApi() = default;

    virtual void CreateWindow(const std::string& window_name, uint32 width, uint32 height, WindowType type) = 0;
    virtual void CreateContext()                                                                            = 0;
    virtual void DeleteContext()                                                                            = 0;
    virtual void ProcessEvents()                                                                            = 0;
    virtual void UpdateWindow()                                                                             = 0;
    virtual void SetFullScreen(bool full_screen)                                                            = 0;
    virtual bool CheckActiveWindow()                                                                        = 0;
    virtual void ShowCursor(bool show)                                                                      = 0;
    virtual std::shared_ptr<Input::InputManager> CreateInput()                                              = 0;
    virtual double GetTime()                                                                                = 0;
    virtual void SetCursorPosition(int x, int y)                                                            = 0;
    virtual uint32 OpenFont(const std::string& filename, uint32 size)                                       = 0;
    virtual Surface RenderFont(uint32 id, const std::string& text, const vec4& color, uint32 outline)       = 0;
    // Take time function to lock fps, shuld be on begin main loop
    virtual void BeginFrame() = 0;
};
typedef std::shared_ptr<IWindowApi> IWindowApiPtr;
}  // namespace GraphicsApi
