#include "KeyCodeToCharConverter.h"

namespace Input
{
// clang-format off
std::unordered_map<KeyCodes::Type, char> KeyCodeToCharConverter::specialCharKeys =
{
    { KeyCodes::SLASH, '/' },
    { KeyCodes::BACKSLASH, '\\' },
    { KeyCodes::MINUS, '-' },
    { KeyCodes::EQUALS, '=' },
    { KeyCodes::LEFTBRACKET, '(' },
    { KeyCodes::RIGHTBRACKET, ')' },
    { KeyCodes::COMMA, ',' },
    { KeyCodes::PERIOD, '.' },
    { KeyCodes::SEMICOLON, ';' },
    { KeyCodes::APOSTROPHE, '\'' },
    { KeyCodes::GRAVE, '~' }
};

std::unordered_map<KeyCodes::Type, char> KeyCodeToCharConverter::smallCharKeys =
{
{ KeyCodes::Q, 'q' },
{ KeyCodes::W, 'w' },
{ KeyCodes::E, 'e' },
{ KeyCodes::R, 'r' },
{ KeyCodes::T, 't' },
{ KeyCodes::Y, 'y' },
{ KeyCodes::U, 'u' },
{ KeyCodes::I, 'i' },
{ KeyCodes::O, 'o' },
{ KeyCodes::P, 'p' },
{ KeyCodes::A, 'a' },
{ KeyCodes::S, 's' },
{ KeyCodes::D, 'd' },
{ KeyCodes::F, 'f' },
{ KeyCodes::G, 'g' },
{ KeyCodes::H, 'h' },
{ KeyCodes::J, 'j' },
{ KeyCodes::K, 'k' },
{ KeyCodes::L, 'l' },
{ KeyCodes::Z, 'z' },
{ KeyCodes::X, 'x' },
{ KeyCodes::C, 'c' },
{ KeyCodes::V, 'v' },
{ KeyCodes::B, 'b' },
{ KeyCodes::N, 'n' },
{ KeyCodes::M, 'm' },
};

std::unordered_map<KeyCodes::Type, char> KeyCodeToCharConverter::bigCharKeys =
{
{ KeyCodes::Q, 'Q' },
{ KeyCodes::W, 'W' },
{ KeyCodes::E, 'E' },
{ KeyCodes::R, 'R' },
{ KeyCodes::T, 'T' },
{ KeyCodes::Y, 'Y' },
{ KeyCodes::U, 'U' },
{ KeyCodes::I, 'I' },
{ KeyCodes::O, 'O' },
{ KeyCodes::P, 'P' },
{ KeyCodes::A, 'A' },
{ KeyCodes::S, 'S' },
{ KeyCodes::D, 'D' },
{ KeyCodes::F, 'F' },
{ KeyCodes::G, 'G' },
{ KeyCodes::H, 'H' },
{ KeyCodes::J, 'J' },
{ KeyCodes::K, 'K' },
{ KeyCodes::L, 'L' },
{ KeyCodes::Z, 'Z' },
{ KeyCodes::X, 'X' },
{ KeyCodes::C, 'C' },
{ KeyCodes::V, 'V' },
{ KeyCodes::B, 'B' },
{ KeyCodes::N, 'N' },
{ KeyCodes::M, 'M' }
};
// clang-format on
std::optional<char> KeyCodeToCharConverter::Convert(KeyCodes::Type t, SingleCharType type)
{
    if (specialCharKeys.count(t) > 0)
    {
        return specialCharKeys.at(t);
    }

    if (type == SingleCharType::SMALL)
    {
        if (smallCharKeys.count(t) == 0)
            return {};

        return smallCharKeys.at(t);
    }

    if (bigCharKeys.count(t) == 0)
        return {};

    return bigCharKeys.at(t);
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
