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

KeysMap KeyConverter::keys = KeysMap({// Mysz (używając Twoich customowych kodów WxKeySpecialKodes)
                                      {KeyCodes::LMOUSE, WxKeySpecialKodes::WX_MOUSE_LEFT},
                                      {KeyCodes::RMOUSE, WxKeySpecialKodes::WX_MOUSE_RIGHT},
                                      {KeyCodes::MOUSE_WHEEL, WxKeySpecialKodes::WX_MOUSE_MIDDLE},

                                      // Litery (ASCII - zawsze wielkie litery w wxWidgets dla GetKeyCode)
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

                                      // Cyfry główne (ASCII)
                                      {KeyCodes::K0, '0'},
                                      {KeyCodes::K1, '1'},
                                      {KeyCodes::K2, '2'},
                                      {KeyCodes::K3, '3'},
                                      {KeyCodes::K4, '4'},
                                      {KeyCodes::K5, '5'},
                                      {KeyCodes::K6, '6'},
                                      {KeyCodes::K7, '7'},
                                      {KeyCodes::K8, '8'},
                                      {KeyCodes::K9, '9'},

                                      // Klawiatura numeryczna (WXK_NUMPAD*)
                                      {KeyCodes::NUM0, 324},
                                      {KeyCodes::NUM1, 325},
                                      {KeyCodes::NUM2, 326},
                                      {KeyCodes::NUM3, 327},
                                      {KeyCodes::NUM4, 328},
                                      {KeyCodes::NUM5, 329},
                                      {KeyCodes::NUM6, 330},
                                      {KeyCodes::NUM7, 331},
                                      {KeyCodes::NUM8, 332},
                                      {KeyCodes::NUM9, 333},
                                      {KeyCodes::NUM_ADD, 334},
                                      {KeyCodes::NUM_SUB, 335},
                                      {KeyCodes::NUM_MULT, 336},
                                      {KeyCodes::NUM_DIV, 337},
                                      {KeyCodes::NUM_DEC, 338},

                                      // Modyfikatory (WXK_CONTROL, WXK_ALT, WXK_SHIFT)
                                      {KeyCodes::LCTRL, 308},
                                      {KeyCodes::RCTRL, 308},
                                      {KeyCodes::LALT, 307},
                                      {KeyCodes::RALT, 307},
                                      {KeyCodes::LSHIFT, 306},
                                      {KeyCodes::RSHIFT, 306},

                                      // Klawisze sterujące (Standardowe kody WXK)
                                      {KeyCodes::ENTER, 13},
                                      {KeyCodes::SPACE, 32},
                                      {KeyCodes::ESCAPE, 27},
                                      {KeyCodes::TAB, 9},
                                      {KeyCodes::BACKSPACE, 8},
                                      {KeyCodes::DEL, 127},
                                      {KeyCodes::INSERT, 322},
                                      {KeyCodes::HOME, 313},
                                      {KeyCodes::END, 312},
                                      {KeyCodes::PAGE_UP, 310},
                                      {KeyCodes::PAGE_DOWN, 311},
                                      {KeyCodes::CAPS_LOCK, 305},

                                      // Strzałki
                                      {KeyCodes::LARROW, 314},
                                      {KeyCodes::RARROW, 316},
                                      {KeyCodes::UARROW, 315},
                                      {KeyCodes::DARROW, 317},

                                      // Funkcyjne F1-F12
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

                                      // Znaki specjalne i OEM
                                      {KeyCodes::SLASH, '/'},
                                      {KeyCodes::OEM_2, '/'},
                                      {KeyCodes::BACKSLASH, '\\'},
                                      {KeyCodes::MINUS, '-'},
                                      {KeyCodes::OEM_MINUS, '-'},
                                      {KeyCodes::EQUALS, '='},
                                      {KeyCodes::OEM_PLUS, '='},
                                      {KeyCodes::LEFTBRACKET, '['},
                                      {KeyCodes::RIGHTBRACKET, ']'},
                                      {KeyCodes::COMMA, ','},
                                      {KeyCodes::OEM_COMMA, ','},
                                      {KeyCodes::PERIOD, '.'},
                                      {KeyCodes::OEM_PERIOD, '.'},
                                      {KeyCodes::SEMICOLON, ';'},
                                      {KeyCodes::OEM_1, ';'},
                                      {KeyCodes::APOSTROPHE, '\''},
                                      {KeyCodes::GRAVE, '`'},
                                      {KeyCodes::OEM_3, '`'},
                                      {KeyCodes::TILD, '`'}});

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

void WxInputManager::ShowCursor(bool show)
{
    isCursorVisible = show;
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
