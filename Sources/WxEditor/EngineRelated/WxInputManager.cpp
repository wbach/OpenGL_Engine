#include "WxInputManager.h"

#include <Logger/Log.h>
#include <wx/event.h>

#include "BidirectionalUnorderedMap.h"
#include "Types.h"
#include "WxKeyEventType.h"

namespace WxEditor
{
namespace
{

typedef BidirectionalUnorderedMap<KeyCodes::Type, uint32> KeysMap;

struct KeyConverter
{
    static KeyCodes::Type Convert(uint32);
    static uint32 Convert(KeyCodes::Type);
    static KeysMap keys;
};

KeysMap KeyConverter::keys = KeysMap({// Mysz
                                      {KeyCodes::LMOUSE, WxKeySpecialKodes::WX_MOUSE_LEFT},
                                      {KeyCodes::RMOUSE, WxKeySpecialKodes::WX_MOUSE_RIGHT},
                                      {KeyCodes::MOUSE_WHEEL, WxKeySpecialKodes::WX_MOUSE_MIDDLE},

                                      // Litery (ASCII)
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

                                      // Cyfry (ASCII)
                                      {KeyCodes::NUM0, '0'},
                                      {KeyCodes::NUM1, '1'},
                                      {KeyCodes::NUM2, '2'},
                                      {KeyCodes::NUM3, '3'},
                                      {KeyCodes::NUM4, '4'},
                                      {KeyCodes::NUM5, '5'},
                                      {KeyCodes::NUM6, '6'},
                                      {KeyCodes::NUM7, '7'},
                                      {KeyCodes::NUM8, '8'},
                                      {KeyCodes::NUM9, '9'},

                                      // Klawisze modyfikujące (WXK_*)
                                      {KeyCodes::LCTRL, 308},
                                      {KeyCodes::LALT, 307},
                                      {KeyCodes::LSHIFT, 306},
                                      {KeyCodes::RSHIFT, 306},  // WX nie rozróżnia L/R w prosty sposób

                                      // Klawisze sterujące
                                      {KeyCodes::ENTER, 13},     // Enter / Return
                                      {KeyCodes::SPACE, 32},     // Spacja
                                      {KeyCodes::ESCAPE, 27},    // Escape
                                      {KeyCodes::TAB, 9},        // Tab
                                      {KeyCodes::BACKSPACE, 8},  // Backspace
                                      {KeyCodes::DEL, 127},      // Delete

                                      // Strzałki (WXK_*)
                                      {KeyCodes::LARROW, 314},
                                      {KeyCodes::RARROW, 316},
                                      {KeyCodes::UARROW, 315},
                                      {KeyCodes::DARROW, 317},

                                      // Funkcyjne (WXK_F1 ... WXK_F12)
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

                                      // Znaki specjalne (ASCII)
                                      {KeyCodes::SLASH, '/'},
                                      {KeyCodes::BACKSLASH, '\\'},
                                      {KeyCodes::MINUS, '-'},
                                      {KeyCodes::EQUALS, '='},
                                      {KeyCodes::LEFTBRACKET, '['},
                                      {KeyCodes::RIGHTBRACKET, ']'},
                                      {KeyCodes::COMMA, ','},
                                      {KeyCodes::PERIOD, '.'},
                                      {KeyCodes::SEMICOLON, ';'},
                                      {KeyCodes::APOSTROPHE, '\''},
                                      {KeyCodes::GRAVE, '`'}});

KeyCodes::Type KeyConverter::Convert(uint32 type)
{
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
}  // namespace

WxInputManager::WxInputManager(const vec2i& windowSize, WrapPointerFunc wrapPointerFunc)
    : windowSize{windowSize}
    , wrapPointer{wrapPointerFunc}
{
    lastMousePosition = vec2i{};
    for (auto& k : keyBuffer)
    {
        k = false;
    }
}

void WxInputManager::SetWindowSize(const vec2i& size)
{
    windowSize = size;
}

void WxInputManager::OnMouseMove(wxMouseEvent& event)
{
    auto currentMousePosition = event.GetPosition();
    lastMouseMove.x           = currentMousePosition.x - lastMousePosition.x;
    lastMouseMove.y           = currentMousePosition.y - lastMousePosition.y;
    lastMousePosition         = vec2i{currentMousePosition.x, currentMousePosition.y};
}

bool WxInputManager::GetKey(KeyCodes::Type code)
{
    return keyBuffer[KeyConverter::Convert(code)];
}

bool WxInputManager::GetMouseKey(KeyCodes::Type code)
{
    return keyBuffer[KeyConverter::Convert(code)];
}

void WxInputManager::SetReleativeMouseMode(bool v)
{
    isRelativeMouseMode = v;
}

vec2i WxInputManager::CalcualteMouseMove()
{
    auto result   = lastMouseMove;
    lastMouseMove = vec2i(0);

    if (isRelativeMouseMode)
    {
        wrapPointer(windowSize.x / 2, windowSize.y / 2);
    }
    return result;
}

vec2i WxInputManager::GetPixelMousePosition()
{
    return lastMousePosition;
}

vec2 WxInputManager::GetMousePosition()
{
    vec2 out;
    out.x = 2.f * (static_cast<float>(lastMousePosition.x) / static_cast<float>(windowSize.x)) - 1.f;
    out.y = 2.f * (static_cast<float>(lastMousePosition.y) / static_cast<float>(windowSize.y)) - 1.f;
    out.y *= -1.f;
    return out;
}

void WxInputManager::SetKeyToBuffer(Input::KeyInteger i, bool v)
{
    keyBuffer[i.value] = v;
}

void WxInputManager::ClearKeyBuffer()
{
    for (auto& key : keyBuffer)
    {
        key = false;
    }
}

void WxInputManager::SetCursorPosition(int x, int y)
{
    if (wrapPointer)
        wrapPointer(x, y);
}

void WxInputManager::GetPressedKeys()
{
}

void WxInputManager::ShowCursor(bool)
{
}

KeyCodes::Type WxInputManager::ConvertCode(uint32 value) const
{
    return KeyConverter::Convert(value);
}

bool WxInputManager::IsKeyUpEventType(uint32 x) const
{
    return x == WxKeyEventType::WX_KEY_UP;
}

bool WxInputManager::IsKeyDownEventType(uint32 x) const
{
    return x == WxKeyEventType::WX_KEY_DOWN;
}

}  // namespace WxEditor
