#pragma once
#include <unordered_map>
#include "Messages/GetCharacters/CharacterInfo.h"
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
    Network::CharacterInfo characterInfo;
    //std::unordered_map<TextType, GameEngine::GuiTextElement> texts;
};
}  // MmmoRpg
