#pragma once
#include <memory>
#include <optional>

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

    virtual void Init()                                                                                         = 0;
    virtual void CreateGameWindow(const std::string& window_name, uint32 width, uint32 height, WindowType type) = 0;
    virtual void CreateContext()                                                                                = 0;
    virtual void DeleteContext()                                                                                = 0;
    virtual void ProcessEvents()                                                                                = 0;
    virtual void UpdateWindow()                                                                                 = 0;
    virtual void SetFullScreen(bool full_screen)                                                                = 0;
    virtual bool CheckActiveWindow()                                                                            = 0;
    virtual void ShowCursor(bool show)                                                                          = 0;
    virtual std::unique_ptr<Input::InputManager> CreateInput()                                                  = 0;
    virtual double GetTime()                                                                                    = 0;
    virtual void SetCursorPosition(int x, int y)                                                                = 0;
    // Take time function to lock fps, shuld be on begin main loop
    virtual void BeginFrame() = 0;
};
}  // namespace GraphicsApi
