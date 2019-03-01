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
                        {KeyCodes::TAB, VK_TAB},
                        {KeyCodes::LSHIFT, VK_LSHIFT},
                        {KeyCodes::RSHIFT, VK_RSHIFT},
                        {KeyCodes::BACKSPACE, VK_BACK}});

KeyCodes::Type Convert(uint32 type)
{
    return keys[type];
}

uint32 Convert(KeyCodes::Type type)
{
    return keys[type];
}
const KeysMap& GetKeysMap()
{
    return keys;
}
}  // namespace WinApiKeyConverter
}  // namespace DirectX
