#include "Gateway.h"
#include "Logger/Log.h"
#include <iostream>

int main(int argc, char** argv)
{
	CLogger::Instance().EnableLogs();	

	Log("RPG GameServer v 0.1");

	SDL_Init(SDL_INIT_EVERYTHING);
	Network::CGateway gateway;
	gateway.StartServer(30, 1991);
	int a;
	std::cin >> a;
	gateway.Quit();

	SDL_Quit();
	return 0;
}