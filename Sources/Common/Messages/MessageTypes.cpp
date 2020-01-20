#include "MessageTypes.h"

#define Return(x) if (type == x) return (#x)
std::string Network::to_string(uint8 type)
{
	Return(Any);
	Return(Ping);
	Return(TransformReq);
	Return(TransformResp);
	Return(Authentication);
	Return(ConnectionMsg);
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
    Return(Other);
	return std::to_string(type);
}
