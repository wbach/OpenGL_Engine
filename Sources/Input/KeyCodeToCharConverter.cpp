#include "KeyCodeToCharConverter.h"

#include "Input/KeyCodes.h"

namespace Input
{
// clang-format off
std::unordered_map<KeyCodes::Type, char> KeyCodeToCharConverter::specialCharKeys =
{
    //{ KeyCodes::SPACE,     ' '  },
    //{ KeyCodes::ENTER,     '\n' },
    { KeyCodes::TAB,       '\t' },
    { KeyCodes::BACKSPACE, '\b' },
    // Numpad zazwyczaj nie zmienia się pod wpływem Shifta w kontekście znaków
    { KeyCodes::NUM0,      '0'  },
    { KeyCodes::NUM1,      '1'  },
    { KeyCodes::NUM2,      '2'  },
    { KeyCodes::NUM3,      '3'  },
    { KeyCodes::NUM4,      '4'  },
    { KeyCodes::NUM5,      '5'  },
    { KeyCodes::NUM6,      '6'  },
    { KeyCodes::NUM7,      '7'  },
    { KeyCodes::NUM8,      '8'  },
    { KeyCodes::NUM9,      '9'  },
    { KeyCodes::NUM_ADD,   '+'  },
    { KeyCodes::NUM_SUB,   '-'  },
    { KeyCodes::NUM_MULT,  '*'  },
    { KeyCodes::NUM_DIV,   '/'  },
    { KeyCodes::NUM_DEC,   '.'  }
};

std::unordered_map<KeyCodes::Type, char> KeyCodeToCharConverter::smallCharKeys =
{
    // Litery
    { KeyCodes::Q, 'q' }, { KeyCodes::W, 'w' }, { KeyCodes::E, 'e' }, { KeyCodes::R, 'r' },
    { KeyCodes::T, 't' }, { KeyCodes::Y, 'y' }, { KeyCodes::U, 'u' }, { KeyCodes::I, 'i' },
    { KeyCodes::O, 'o' }, { KeyCodes::P, 'p' }, { KeyCodes::A, 'a' }, { KeyCodes::S, 's' },
    { KeyCodes::D, 'd' }, { KeyCodes::F, 'f' }, { KeyCodes::G, 'g' }, { KeyCodes::H, 'h' },
    { KeyCodes::J, 'j' }, { KeyCodes::K, 'k' }, { KeyCodes::L, 'l' }, { KeyCodes::Z, 'z' },
    { KeyCodes::X, 'x' }, { KeyCodes::C, 'c' }, { KeyCodes::V, 'v' }, { KeyCodes::B, 'b' },
    { KeyCodes::N, 'n' }, { KeyCodes::M, 'm' },

    // Cyfry i znaki (BEZ SHIFTA)
    { KeyCodes::K0, '0' }, { KeyCodes::K1, '1' }, { KeyCodes::K2, '2' }, { KeyCodes::K3, '3' },
    { KeyCodes::K4, '4' }, { KeyCodes::K5, '5' }, { KeyCodes::K6, '6' }, { KeyCodes::K7, '7' },
    { KeyCodes::K8, '8' }, { KeyCodes::K9, '9' },
    { KeyCodes::MINUS,        '-'  },
    { KeyCodes::EQUALS,       '='  },
    { KeyCodes::LEFTBRACKET,  '['  },
    { KeyCodes::RIGHTBRACKET, ']'  },
    { KeyCodes::SEMICOLON,    ';'  },
    { KeyCodes::APOSTROPHE,   '\'' },
    { KeyCodes::COMMA,        ','  },
    { KeyCodes::PERIOD,       '.'  },
    { KeyCodes::SLASH,        '/'  },
    { KeyCodes::BACKSLASH,    '\\' },
    { KeyCodes::GRAVE,        '`'  },
    { KeyCodes::TILD,         '`'  }, // Zazwyczaj to ten sam klawisz co GRAVE
    { KeyCodes::OEM_MINUS,    '-'  },
    { KeyCodes::OEM_PLUS,     '='  },
    { KeyCodes::OEM_COMMA,    ','  },
    { KeyCodes::OEM_PERIOD,   '.'  }
};

std::unordered_map<KeyCodes::Type, char> KeyCodeToCharConverter::bigCharKeys =
{
    // Litery (Z SHIFTEM)
    { KeyCodes::Q, 'Q' }, { KeyCodes::W, 'W' }, { KeyCodes::E, 'E' }, { KeyCodes::R, 'R' },
    { KeyCodes::T, 'T' }, { KeyCodes::Y, 'Y' }, { KeyCodes::U, 'U' }, { KeyCodes::I, 'I' },
    { KeyCodes::O, 'O' }, { KeyCodes::P, 'P' }, { KeyCodes::A, 'A' }, { KeyCodes::S, 'S' },
    { KeyCodes::D, 'D' }, { KeyCodes::F, 'F' }, { KeyCodes::G, 'G' }, { KeyCodes::H, 'H' },
    { KeyCodes::J, 'J' }, { KeyCodes::K, 'K' }, { KeyCodes::L, 'L' }, { KeyCodes::Z, 'Z' },
    { KeyCodes::X, 'X' }, { KeyCodes::C, 'C' }, { KeyCodes::V, 'V' }, { KeyCodes::B, 'B' },
    { KeyCodes::N, 'N' }, { KeyCodes::M, 'M' },

    // Znaki specjalne (Z SHIFTEM)
    { KeyCodes::K1, '!' }, { KeyCodes::K2, '@' }, { KeyCodes::K3, '#' }, { KeyCodes::K4, '$' },
    { KeyCodes::K5, '%' }, { KeyCodes::K6, '^' }, { KeyCodes::K7, '&' }, { KeyCodes::K8, '*' },
    { KeyCodes::K9, '(' }, { KeyCodes::K0, ')' },
    { KeyCodes::MINUS,        '_'  },
    { KeyCodes::EQUALS,       '+'  },
    { KeyCodes::LEFTBRACKET,  '{'  },
    { KeyCodes::RIGHTBRACKET, '}'  },
    { KeyCodes::SEMICOLON,    ':'  },
    { KeyCodes::APOSTROPHE,   '\"' },
    { KeyCodes::COMMA,        '<'  },
    { KeyCodes::PERIOD,       '>'  },
    { KeyCodes::SLASH,        '?'  },
    { KeyCodes::BACKSLASH,    '|'  },
    { KeyCodes::GRAVE,        '~'  },
    { KeyCodes::TILD,         '~'  },
    { KeyCodes::OEM_MINUS,    '_'  },
    { KeyCodes::OEM_PLUS,     '+'  },
    { KeyCodes::OEM_COMMA,    '<'  },
    { KeyCodes::OEM_PERIOD,   '>'  }
};
// clang-format on

std::optional<char> KeyCodeToCharConverter::Convert(KeyCodes::Type t, SingleCharType type)
{
    auto specilIter = specialCharKeys.find(t);
    if (specilIter != specialCharKeys.end())
    {
        return specilIter->second;
    }

    if (type == SingleCharType::SMALL)
    {
        auto smallIter = smallCharKeys.find(t);
        if (smallIter == smallCharKeys.end())
            return {};

        return smallIter->second;
    }

    auto bigIter = bigCharKeys.find(t);
    if (bigIter == bigCharKeys.end())
        return {};

    return bigIter->second;
}

KeyCodes::Type KeyCodeToCharConverter::Convert(char ch)
{
    for (const auto& c : specialCharKeys)
    {
        if (c.second == ch)
            return c.first;
    }
    for (const auto& c : smallCharKeys)
    {
        if (c.second == ch)
            return c.first;
    }
    for (const auto& c : bigCharKeys)
    {
        if (c.second == ch)
            return c.first;
    }
    return KeyCodes::Type();
}
}  // namespace Input
