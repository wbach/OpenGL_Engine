#pragma once

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
} // Network
