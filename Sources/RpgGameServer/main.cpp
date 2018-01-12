#include "Gateway.h"
#include "Logger/Log.h"


int main(int argc, char** argv)
{
	CLogger::Instance().EnableLogs();
	
	Log("RPG GameServer v 0.1");	
	Network::CGateway gateway;
	gateway.StartServer(30, 1991);
	return 0;
}