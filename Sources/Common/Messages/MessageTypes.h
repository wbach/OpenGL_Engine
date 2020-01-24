#pragma once
#include "Types.h"

namespace common
{
enum MessageTypes
{
    Any,
    TransformReq,
    TransformResp,
    DisconnectCharacter,
    SelectCharacterReq,
    SelectCharacterResp,
    CreateCharacterReq,
    CreateCharacterResp,
    GetCharactersReq,
    GetCharactersResp,
    GetCharacterDataReq,
    GetCharacterDataResp,
    GetCharactersDataReq
};
std::string to_string(uint8 type);
#define COMMON_MESSAGE_TYPE_RANGE_LOW MessageTypes::Any
#define COMMON_MESSAGE_TYPE_RANGE_HIGH MessageTypes::GetCharactersDataReq
}  // namespace Network
