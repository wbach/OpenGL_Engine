#pragma once
#include "Messages/GetCharacters/CharacterInfo.h"
#include "Renderers/GUI/Text/GuiTextElement.h"
#include <unordered_map>

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
		std::unordered_map<TextType, SGuiTextElement> texts;
	};
} // MmmoRpg