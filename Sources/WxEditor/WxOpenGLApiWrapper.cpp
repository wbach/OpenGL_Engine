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
//#include "../../Input/KeyCodes.h"
#include "BidirectionalUnorderedMap.h"
#include "Types.h"

typedef BidirectionalUnorderedMap<KeyCodes::Type, uint32> KeysMap;

struct KeyConverter
{
    static KeyCodes::Type Convert(uint32);
    static uint32 Convert(KeyCodes::Type);
    static KeysMap keys;
};

KeysMap KeyConverter::keys = KeysMap({
    //    {KeyCodes::LMOUSE, SDL_BUTTON_LEFT},
    //    {KeyCodes::RMOUSE, SDL_BUTTON_RIGHT},
    //    {KeyCodes::MOUSE_WHEEL, SDL_BUTTON_MIDDLE},
    {KeyCodes::Q, 'Q'},
    {KeyCodes::W, 'W'},
    {KeyCodes::E, 'E'},
    {KeyCodes::R, 'R'},
    {KeyCodes::T, 'T'},
    {KeyCodes::Y, 'Y'},
    {KeyCodes::U, 'U'},
    {KeyCodes::I, 'I'},
    {KeyCodes::O, 'O'},
    {KeyCodes::P, 'P'},
    {KeyCodes::A, 'A'},
    {KeyCodes::S, 'S'},
    {KeyCodes::D, 'D'},
    {KeyCodes::F, 'F'},
    {KeyCodes::G, 'G'},
    {KeyCodes::H, 'H'},
    {KeyCodes::J, 'J'},
    {KeyCodes::K, 'K'},
    {KeyCodes::L, 'L'},
    {KeyCodes::Z, 'Z'},
    {KeyCodes::X, 'X'},
    {KeyCodes::C, 'C'},
    {KeyCodes::V, 'V'},
    {KeyCodes::B, 'B'},
    {KeyCodes::N, 'N'},
    {KeyCodes::M, 'M'},
    {KeyCodes::LALT, 307},
    {KeyCodes::LCTRL, 308},
    {KeyCodes::ENTER, 13},
    {KeyCodes::SPACE, 32},
    {KeyCodes::ESCAPE, 27},
    {KeyCodes::LARROW, 314},
    {KeyCodes::RARROW, 316},
    {KeyCodes::UARROW, 315},
    {KeyCodes::DARROW, 317},
    {KeyCodes::F1, 340},
    {KeyCodes::F2, 341},
    {KeyCodes::F3, 342},
    {KeyCodes::F4, 343},
    {KeyCodes::F5, 344},
    {KeyCodes::F6, 345},
    {KeyCodes::F7, 346},
    {KeyCodes::F8, 347},
    {KeyCodes::F9, 348},
    {KeyCodes::F10, 349},
    {KeyCodes::F11, 350},
    {KeyCodes::F12, 351},
    {KeyCodes::TAB, 9},
    {KeyCodes::LSHIFT, 306},
    {KeyCodes::RSHIFT, 306},  // ?;p
    {KeyCodes::SLASH, 47},
    {KeyCodes::BACKSLASH, 92},
    //    {KeyCodes::MINUS, SDL_SCANCODE_MINUS},
    //    {KeyCodes::EQUALS, SDL_SCANCODE_EQUALS},
    //    {KeyCodes::LEFTBRACKET, SDL_SCANCODE_LEFTBRACKET},
    //    {KeyCodes::RIGHTBRACKET, SDL_SCANCODE_RIGHTBRACKET},
    //    {KeyCodes::COMMA, SDL_SCANCODE_COMMA},
    //    {KeyCodes::PERIOD, SDL_SCANCODE_PERIOD},
    //    {KeyCodes::SEMICOLON, SDL_SCANCODE_SEMICOLON},
    //    {KeyCodes::APOSTROPHE, SDL_SCANCODE_APOSTROPHE},
    //    {KeyCodes::GRAVE, SDL_SCANCODE_GRAVE},
    {KeyCodes::BACKSPACE, 8},
    //    {KeyCodes::NUM1, SDL_SCANCODE_1},
    //    {KeyCodes::NUM2, SDL_SCANCODE_2},
    //    {KeyCodes::NUM3, SDL_SCANCODE_3},
    //    {KeyCodes::NUM4, SDL_SCANCODE_4},
    //    {KeyCodes::NUM5, SDL_SCANCODE_5},
    //    {KeyCodes::NUM6, SDL_SCANCODE_6},
    //    {KeyCodes::NUM7, SDL_SCANCODE_7},
    //    {KeyCodes::NUM8, SDL_SCANCODE_8},
    //    {KeyCodes::NUM9, SDL_SCANCODE_9},
    //    {KeyCodes::NUM0, SDL_SCANCODE_0},
    {KeyCodes::DEL, 127}  //
});

KeyCodes::Type KeyConverter::Convert(uint32 type)
{
    DEBUG_LOG("Input: " + std::to_string(type) + " /" + std::to_string(uint32('Z')));
    if (keys.Count(type) <= 0)
        return KeyCodes::Type::None;

    return keys[type];
}

uint32 KeyConverter::Convert(KeyCodes::Type type)
{
    if (keys.Count(type) <= 0)
        return 0;

    return keys[type];
}

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
    KeyCodes::Type ConvertCode(uint32 value) const
    {
        return KeyConverter::Convert(value);
    }
    bool IsKeyUpEventType(uint32 x) const
    {
        return x == WxKeyEventType::WX_KEY_UP;
    }
    bool IsKeyDownEventType(uint32 x) const
    {
        return x == WxKeyEventType::WX_KEY_DOWN;
    }
};
}  // namespace

class WxWindowApi : public GraphicsApi::IWindowApi
{
public:
    WxWindowApi(const vec2i renderSize)
    {
        DEBUG_LOG("Add display mode: " + std::to_string(renderSize));
        displayModes.push_back(
            GraphicsApi::DisplayMode{.w = renderSize.x, .h = renderSize.y, .refreshRate = 60, .displayIndex = 0});
    }
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

WxOpenGLApiWrapper::WxOpenGLApiWrapper(const vec2i renderSize)
    : OpenGLApi::OpenGLApi(std::make_unique<WxWindowApi>(renderSize))
{
    SetBackgroundColor(Color(0.18f, 0.27f, 0.47f));
}

void WxOpenGLApiWrapper::CreateContext()
{
    DEBUG_LOG("Skip createContext, should already be created");
}

}  // namespace WxEditor
