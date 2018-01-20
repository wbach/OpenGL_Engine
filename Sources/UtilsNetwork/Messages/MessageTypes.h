#pragma once

namespace Network
{
	enum MessageTypes
	{
		Any,
		Transform,
		Authentication,
		ConnectionMsg,
		SelectCharacterReq,
		SelectCharacterResp,
		CreateCharacterReq,
		CreateCharacterResp,
	};
} // Network
