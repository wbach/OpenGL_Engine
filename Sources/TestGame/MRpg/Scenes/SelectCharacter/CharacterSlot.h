#pragma once
#include <Common/Messages/GetCharacters/CharacterInfo.h>
#include <unordered_map>
#include "Renderers/GUI/Text/GuiTextElement.h"

namespace MmmoRpg
{
struct CharacterSlot
{
    enum TextType
    {
        LVL,
        CLASSNAME,
        NICK
    };
    common::CharacterInfo characterInfo;
    // std::unordered_map<TextType, GameEngine::GuiTextElement> texts;
};
}  // namespace MmmoRpg
