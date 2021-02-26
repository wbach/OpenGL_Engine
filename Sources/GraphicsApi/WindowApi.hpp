#pragma once
#include <memory>
#include <optional>
#include <variant>
#include <vector>

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
struct DisplayMode
{
    int w;
    int h;
    int refreshRate;
    int displayIndex;
};

struct DropFileEvent
{
    std::string filename;
};
struct QuitEvent
{
};

class IWindowApi
{
public:
    using Event = std::variant<DropFileEvent, QuitEvent>;

    virtual ~IWindowApi() = default;

    virtual void Init()                                                                     = 0;
    virtual void CreateGameWindow(const std::string&, uint32, uint32, WindowType)           = 0;
    virtual void SetWindowSize(const vec2ui&)                                               = 0;
    virtual void CreateContext()                                                            = 0;
    virtual void DeleteContext()                                                            = 0;
    virtual void ProcessEvents()                                                            = 0;
    virtual void UpdateWindow()                                                             = 0;
    virtual IdType SubscribeForEvent(std::function<void(const Event&)>)                     = 0;
    virtual void UnsubscribeForEvent(IdType)                                                = 0;
    virtual void SetFullScreen(bool full_screen)                                            = 0;
    virtual bool CheckActiveWindow()                                                        = 0;
    virtual void ShowCursor(bool show)                                                      = 0;
    virtual std::unique_ptr<Input::InputManager> CreateInput()                              = 0;
    virtual double GetTime()                                                                = 0;
    virtual void SetCursorPosition(int x, int y)                                            = 0;
    virtual void ShowMessageBox(const std::string& title, const std::string& message) const = 0;
    virtual void ShowMessageBox(const std::string& title, const std::string& message,
                                std::function<void(bool)>) const                            = 0;
    virtual const std::vector<DisplayMode>& GetDisplayModes() const                         = 0;
    // Take time function to lock fps, shuld be on begin main loop
    virtual void BeginFrame() = 0;
};
}  // namespace GraphicsApi
