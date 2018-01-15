#include "Gateway.h"
#include "Logger/Log.h"
#include <iostream>


class GameServer
{
public:
	GameServer()
	{
		gateway.StartServer(30, 1991);
		gateway.SubscribeForNewUser(std::bind(&GameServer::NewUser, this, std::placeholders::_1));
		Update();
	}
	void Update()
	{
		Network::TestData tdata;
		tdata.position = vec3(1.f, 2.f, 3.f);
		tdata.rotation = vec3(4.f, 5.f, 6.f);

		while (true)
		{
			Log("ServerMainLoop.");
			++tdata.id;
			tdata.position += vec3(0.25, 0.1, 0.2);

			{
				auto m = std::make_shared<Network::TestData>(tdata);

				std::lock_guard<std::mutex> lk(usersMutex);
				for(auto& user : users)
					gateway.AddToOutbox(user, m);
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(200));
		}
	}
private:
	void NewUser(uint32 id)
	{
		std::lock_guard<std::mutex> lk(usersMutex);
		users.push_back(id);
	}
private:
	Network::CGateway gateway;
	std::mutex usersMutex;
	std::list<uint32> users;
};

int main(int argc, char** argv)
{
	CLogger::Instance().EnableLogs();	

	Log("RPG GameServer v 0.1");

	SDL_Init(SDL_INIT_EVERYTHING);	
	GameServer();
	SDL_Quit();
	return 0;
}