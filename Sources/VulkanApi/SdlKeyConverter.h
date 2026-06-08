#pragma once
#include <Input/KeyCodes.h>

#include "BidirectionalUnorderedMap.h"
#include "Types.h"

namespace GraphicsApi::Vulkan
{
typedef BidirectionalUnorderedMap<KeyCodes::Type, uint32> SdlKeysMap;

struct SdlKeyConverter
{
    static KeyCodes::Type Convert(uint32);
    static uint32 Convert(KeyCodes::Type);
    static SdlKeysMap keys;
};
}  // namespace GraphicsApi::Vulkan
