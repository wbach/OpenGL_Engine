#pragma once

namespace Network
{
	enum MessageTypes
	{
		Any,
		Ping,
		Transform,
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
