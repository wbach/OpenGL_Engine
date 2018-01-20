#pragma once

namespace Network
{
	enum ConnectionStatus
	{
		CONNECTED = 1,
		WAIT_FOR_AUTHENTICATION,
		ERROR_FULL,
		ERROR_FAILD_AUTHENTICATION
	};
} // Network
