#include "MessageTypes.h"

#define Return(x)  \
    if (type == x) \
    return (#x)

std::string common::to_string(uint8 type)
{
    Return(Any);
    Return(TransformReq);
    Return(TransformResp);
    Return(SelectCharacterReq);
    Return(SelectCharacterResp);
    Return(CreateCharacterReq);
    Return(CreateCharacterResp);
    Return(GetCharactersReq);
    Return(GetCharactersResp);
    Return(GetCharacterDataReq);
    Return(GetCharacterDataResp);
    Return(GetCharactersDataReq);
    Return(DisconnectCharacter);
    return std::to_string(type);
}
