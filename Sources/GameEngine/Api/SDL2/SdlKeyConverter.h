#pragma once
#include "Types.h"
#include "BidirectionalUnorderedMap.h"
#include "../../Input/KeyCodes.h"

namespace GameEngine
{
	typedef BidirectionalUnorderedMap<KeyCodes::Type, uint32> SdlKeysMap;

	struct SdlKeyConverter
	{
		static KeyCodes::Type Convert(uint32);
		static uint32 Convert(KeyCodes::Type);
		static SdlKeysMap keys;		
	};
} // GameEngine