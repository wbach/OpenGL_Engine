#pragma once
#include "../../Input/KeyCodes.h"
#include "BidirectionalUnorderedMap.h"
#include "Types.h"

namespace OpenGLApi
{
typedef BidirectionalUnorderedMap<KeyCodes::Type, uint32> SdlKeysMap;

struct SdlKeyConverter
{
    static KeyCodes::Type Convert(uint32);
    static uint32 Convert(KeyCodes::Type);
    static SdlKeysMap keys;
};
}  // OpenGLApi
