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
const std::pair<uint8, uint8> MESSAGE_TYPE_RANGE{MessageTypes::TransformReq, MessageTypes::GetCharactersDataReq};
}  // namespace Network
