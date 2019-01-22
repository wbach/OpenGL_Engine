#pragma once
#include <unordered_map>
#include "KeyCodes.h"

namespace Input
{
enum SingleCharType
{
    SMALL,
    BIG
};

class KeyCodeToCharConverter
{
public:
    static char Convert(KeyCodes::Type, SingleCharType = SingleCharType::SMALL);
    static KeyCodes::Type Convert(char);

private:
    static std::unordered_map<KeyCodes::Type, char> smallCharKeys;
    static std::unordered_map<KeyCodes::Type, char> bigCharKeys;
};
}  // namespace Input