#pragma once
#include "Types.h"

namespace Network
{
	struct UserAccount
	{
		uint32 id;
		std::string login;
		std::string password;
	};
}