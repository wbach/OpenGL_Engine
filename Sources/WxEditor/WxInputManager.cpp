#include "WxInputManager.h"

#include "BidirectionalUnorderedMap.h"
#include "Types.h"
#include "WxKeyEventType.h"

#include <Logger/Log.h>

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
}  // namespace

bool WxInputManager::GetKey(KeyCodes::Type)
{
    return false;
}

bool WxInputManager::GetMouseKey(KeyCodes::Type)
{
    return false;
}

void WxInputManager::SetReleativeMouseMode(bool)
{
}

vec2i WxInputManager::CalcualteMouseMove()
{
    return {0, 0};
}

vec2i WxInputManager::GetPixelMousePosition()
{
    return {0, 0};
}

vec2 WxInputManager::GetMousePosition()
{
    return {0, 0};
}

void WxInputManager::SetKeyToBuffer(int, bool)
{
}

void WxInputManager::ClearKeyBuffer()
{
}

void WxInputManager::SetCursorPosition(int, int)
{
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
