#include "WinApiKeyConverter.h"

#include <windows.h>

namespace GraphicsApi::Dx11
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

                        // Alfabetyczne
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

                        // Cyfry Główne (nad literami)
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

                        // Numpad
                        {KeyCodes::NUM0, VK_NUMPAD0},
                        {KeyCodes::NUM1, VK_NUMPAD1},
                        {KeyCodes::NUM2, VK_NUMPAD2},
                        {KeyCodes::NUM3, VK_NUMPAD3},
                        {KeyCodes::NUM4, VK_NUMPAD4},
                        {KeyCodes::NUM5, VK_NUMPAD5},
                        {KeyCodes::NUM6, VK_NUMPAD6},
                        {KeyCodes::NUM7, VK_NUMPAD7},
                        {KeyCodes::NUM8, VK_NUMPAD8},
                        {KeyCodes::NUM9, VK_NUMPAD9},
                        {KeyCodes::NUM_MULT, VK_MULTIPLY},
                        {KeyCodes::NUM_ADD, VK_ADD},
                        {KeyCodes::NUM_SUB, VK_SUBTRACT},
                        {KeyCodes::NUM_DEC, VK_DECIMAL},
                        {KeyCodes::NUM_DIV, VK_DIVIDE},

                        // Funkcyjne
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

                        // Sterowanie i Modyfikatory
                        {KeyCodes::LALT, VK_LMENU},
                        {KeyCodes::RALT, VK_RMENU},
                        {KeyCodes::LCTRL, VK_LCONTROL},
                        {KeyCodes::LSHIFT, VK_LSHIFT},
                        {KeyCodes::RSHIFT, VK_RSHIFT},
                        {KeyCodes::ENTER, VK_RETURN},
                        {KeyCodes::SPACE, VK_SPACE},
                        {KeyCodes::ESCAPE, VK_ESCAPE},
                        {KeyCodes::TAB, VK_TAB},
                        {KeyCodes::BACKSPACE, VK_BACK},
                        {KeyCodes::CAPS_LOCK, VK_CAPITAL},

                        // Nawigacja
                        {KeyCodes::LARROW, VK_LEFT},
                        {KeyCodes::RARROW, VK_RIGHT},
                        {KeyCodes::UARROW, VK_UP},
                        {KeyCodes::DARROW, VK_DOWN},
                        {KeyCodes::INSERT, VK_INSERT},
                        {KeyCodes::DEL, VK_DELETE},
                        {KeyCodes::HOME, VK_HOME},
                        {KeyCodes::END, VK_END},
                        {KeyCodes::PAGE_UP, VK_PRIOR},
                        {KeyCodes::PAGE_DOWN, VK_NEXT},

                        // Interpunkcja (zależna od układu, ale to są standardy)
                        {KeyCodes::OEM_PLUS, VK_OEM_PLUS},      // + / =
                        {KeyCodes::OEM_MINUS, VK_OEM_MINUS},    // - / _
                        {KeyCodes::OEM_COMMA, VK_OEM_COMMA},    // , / <
                        {KeyCodes::OEM_PERIOD, VK_OEM_PERIOD},  // . / >
                        {KeyCodes::OEM_1, VK_OEM_1},            // ; / :
                        {KeyCodes::OEM_2, VK_OEM_2},            // / / ?
                        {KeyCodes::OEM_3, VK_OEM_3},            // ` / ~
                        {KeyCodes::TILD, VK_OEM_3}});

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
}  // namespace GraphicsApi::Dx11
