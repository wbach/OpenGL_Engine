#include "WinApiKeyConverter.h"

#include <Windows.h>

namespace DirectX
{
namespace
{
KeysMap keys;
}
namespace WinApiKeyConverter
{
KeysMap keys = KeysMap({{KeyCodes::LMOUSE, VK_LBUTTON},
                        {KeyCodes::RMOUSE, VK_RBUTTON},
                        {KeyCodes::MOUSE_WHEEL, VK_MBUTTON},
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
                        {KeyCodes::LCTRL, VK_CONTROL},
                        {KeyCodes::ENTER, VK_RETURN},
                        {KeyCodes::SPACE, VK_SPACE},
                        {KeyCodes::ESCAPE, VK_ESCAPE},
                        {KeyCodes::LARROW, VK_LEFT},
                        {KeyCodes::RARROW, VK_RIGHT},
                        {KeyCodes::UARROW, VK_UP},
                        {KeyCodes::DARROW, VK_DOWN},
                        {KeyCodes::F1, VK_F1},
                        {KeyCodes::F2, VK_F2},
                        {KeyCodes::F3, VK_F3},
                        {KeyCodes::F4, VK_F4},
                        {KeyCodes::F5, VK_F5},
                        {KeyCodes::F6, VK_F6},
                        {KeyCodes::F7, VK_F7},
                        {KeyCodes::F8, VK_F8},
                        {KeyCodes::F9, VK_F9},
                        {KeyCodes::F10, VK_F10},
                        {KeyCodes::F11, VK_F11},
                        {KeyCodes::F12, VK_F12},
                        {KeyCodes::TAB, VK_TAB},
                        {KeyCodes::LSHIFT, VK_LSHIFT},
                        {KeyCodes::RSHIFT, VK_RSHIFT},
                        {KeyCodes::BACKSPACE, VK_BACK}});

KeyCodes::Type Convert(uint32 type)
{
    if (keys.Count(type) == 0)
        return KeyCodes::None;

    return keys[type];
}

uint32 Convert(KeyCodes::Type type)
{
    if (keys.Count(type) == 0)
        return 0;

    return keys[type];
}
const KeysMap& GetKeysMap()
{
    return keys;
}
}  // namespace WinApiKeyConverter
}  // namespace DirectX
