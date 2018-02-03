#pragma once
#include "KeyCodes.h"
#include <unordered_map>

namespace GameEngine
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
} // GameEngine