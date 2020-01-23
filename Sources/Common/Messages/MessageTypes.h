#pragma once
#include "Types.h"

namespace common
{
enum MessageTypes
{
    TransformReq = 0,
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
#define COMMON_MESSAGE_TYPE_RANGE_LOW MessageTypes::TransformReq
#define COMMON_MESSAGE_TYPE_RANGE_HIGH MessageTypes::GetCharactersDataReq
}  // namespace Network
