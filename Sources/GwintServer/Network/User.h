#pragma once
#include <SDL2/SDL_net.h>

struct UserData
{
	TCPsocket socket;
	Uint32	  timeout;
	unsigned int id;

	UserData()
		: socket(nullptr)
		, timeout(0)
	{
		static int s_id = 0;
		id = s_id++;
	}
};