#pragma once
#include "BidirectionalUnorderedMap.h"
#include "Input/KeyCodes.h"
#include "Types.h"

namespace DirectX
{
typedef BidirectionalUnorderedMap<KeyCodes::Type, uint32> KeysMap;

namespace WinApiKeyConverter
{
KeyCodes::Type Convert(uint32);
uint32 Convert(KeyCodes::Type);
const KeysMap& GetKeysMap();
}  // namespace WinApiKeyConverter
}  // namespace DirectX
