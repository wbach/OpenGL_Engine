#include "SdlKeyConverter.h"
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_scancode.h>

namespace OpenGLApi
{
SdlKeysMap SdlKeyConverter::keys = SdlKeysMap({
    {KeyCodes::LMOUSE, SDL_BUTTON_LEFT},
    {KeyCodes::RMOUSE, SDL_BUTTON_RIGHT},
    {KeyCodes::MOUSE_WHEEL, SDL_BUTTON_MIDDLE},
    {KeyCodes::Q, SDL_SCANCODE_Q},
    {KeyCodes::W, SDL_SCANCODE_W},
    {KeyCodes::E, SDL_SCANCODE_E},
    {KeyCodes::R, SDL_SCANCODE_R},
    {KeyCodes::T, SDL_SCANCODE_T},
    {KeyCodes::Y, SDL_SCANCODE_Y},
    {KeyCodes::U, SDL_SCANCODE_U},
    {KeyCodes::I, SDL_SCANCODE_I},
    {KeyCodes::O, SDL_SCANCODE_O},
    {KeyCodes::P, SDL_SCANCODE_P},
    {KeyCodes::A, SDL_SCANCODE_A},
    {KeyCodes::S, SDL_SCANCODE_S},
    {KeyCodes::D, SDL_SCANCODE_D},
    {KeyCodes::F, SDL_SCANCODE_F},
    {KeyCodes::G, SDL_SCANCODE_G},
    {KeyCodes::H, SDL_SCANCODE_H},
    {KeyCodes::J, SDL_SCANCODE_J},
    {KeyCodes::K, SDL_SCANCODE_K},
    {KeyCodes::L, SDL_SCANCODE_L},
    {KeyCodes::Z, SDL_SCANCODE_Z},
    {KeyCodes::X, SDL_SCANCODE_X},
    {KeyCodes::C, SDL_SCANCODE_C},
    {KeyCodes::V, SDL_SCANCODE_V},
    {KeyCodes::B, SDL_SCANCODE_B},
    {KeyCodes::N, SDL_SCANCODE_N},
    {KeyCodes::M, SDL_SCANCODE_M},
    {KeyCodes::LCTRL, SDL_SCANCODE_LCTRL},
    {KeyCodes::ENTER, SDL_SCANCODE_RETURN},
    {KeyCodes::SPACE, SDL_SCANCODE_SPACE},
    {KeyCodes::ESCAPE, SDL_SCANCODE_ESCAPE},
    {KeyCodes::LARROW, SDL_SCANCODE_LEFT},
    {KeyCodes::RARROW, SDL_SCANCODE_RIGHT},
    {KeyCodes::UARROW, SDL_SCANCODE_UP},
    {KeyCodes::DARROW, SDL_SCANCODE_DOWN},
    {KeyCodes::F1, SDL_SCANCODE_F1},
    {KeyCodes::F2, SDL_SCANCODE_F2},
    {KeyCodes::F3, SDL_SCANCODE_F3},
    {KeyCodes::F4, SDL_SCANCODE_F4},
    {KeyCodes::F5, SDL_SCANCODE_F5},
    {KeyCodes::F6, SDL_SCANCODE_F6},
    {KeyCodes::F7, SDL_SCANCODE_F7},
    {KeyCodes::F8, SDL_SCANCODE_F8},
    {KeyCodes::F9, SDL_SCANCODE_F9},
    {KeyCodes::F10, SDL_SCANCODE_F10},
    {KeyCodes::F11, SDL_SCANCODE_F11},
    {KeyCodes::F12, SDL_SCANCODE_F12},
    {KeyCodes::TAB, SDL_SCANCODE_TAB},
    {KeyCodes::LSHIFT, SDL_SCANCODE_LSHIFT},
    {KeyCodes::RSHIFT, SDL_SCANCODE_RSHIFT},
    {KeyCodes::SLASH, SDL_SCANCODE_SLASH},
    {KeyCodes::BACKSLASH, SDL_SCANCODE_BACKSLASH},
    {KeyCodes::MINUS, SDL_SCANCODE_MINUS},
    {KeyCodes::EQUALS, SDL_SCANCODE_EQUALS},
    {KeyCodes::LEFTBRACKET, SDL_SCANCODE_LEFTBRACKET},
    {KeyCodes::RIGHTBRACKET, SDL_SCANCODE_RIGHTBRACKET},
    {KeyCodes::COMMA, SDL_SCANCODE_COMMA},
    {KeyCodes::PERIOD, SDL_SCANCODE_PERIOD},
    {KeyCodes::SEMICOLON, SDL_SCANCODE_SEMICOLON},
    {KeyCodes::APOSTROPHE, SDL_SCANCODE_APOSTROPHE},
    {KeyCodes::GRAVE, SDL_SCANCODE_GRAVE},
    {KeyCodes::BACKSPACE, SDL_SCANCODE_BACKSPACE},
    {KeyCodes::NUM1, SDL_SCANCODE_1},
    {KeyCodes::NUM2, SDL_SCANCODE_2},
    {KeyCodes::NUM3, SDL_SCANCODE_3},
    {KeyCodes::NUM4, SDL_SCANCODE_4},
    {KeyCodes::NUM5, SDL_SCANCODE_5},
    {KeyCodes::NUM6, SDL_SCANCODE_6},
    {KeyCodes::NUM7, SDL_SCANCODE_7},
    {KeyCodes::NUM8, SDL_SCANCODE_8},
    {KeyCodes::NUM9, SDL_SCANCODE_9},
    {KeyCodes::NUM0, SDL_SCANCODE_0}
});

KeyCodes::Type SdlKeyConverter::Convert(uint32 type)
{
    if (keys.Count(type) <= 0)
        return KeyCodes::Type::None;

    return keys[type];
}

uint32 SdlKeyConverter::Convert(KeyCodes::Type type)
{
    if (keys.Count(type) <= 0)
        return 0;

    return keys[type];
}
}  // namespace OpenGLApi
