#pragma once
#include "Types.h"

namespace Network
{
	enum MessageTypes
	{
		Any,
		Ping,
		TransformReq,
		TransformResp,
		Authentication,
		ConnectionMsg,
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
} // Network
