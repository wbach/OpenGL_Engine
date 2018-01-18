#include "GameServer.h"
#include <functional>
#include "Logger/Log.h"
#include "Database/DatabaseWrapperMock.h"

namespace GameServer
{
	GameServer::GameServer()
		: running_(true)
		, gateway_(new Network::CGateway())
		//, context_(new Database::DatabaseWrapperMock(), std::bind(&Network::CGateway::AddToOutbox, gateway_.get(), std::placeholders::_1, std::placeholders::_2))
	{
		context_.databaseWrapper_ = std::make_shared<Database::DatabaseWrapperMock()>();
		context_.sendMessage_ = std::bind(&Network::CGateway::AddToOutbox, gateway_.get(), std::placeholders::_1, std::placeholders::_2);
		gateway_->StartServer(30, 1991);
		gateway_->SubscribeForNewUser(std::bind(&GameServer::NewUser, this, std::placeholders::_1, std::placeholders::_2));
		gateway_->SubscribeOnMessageArrived(std::bind(&GameServer::OnMessageArrived, this, std::placeholders::_1));
		Update();
	}
	void GameServer::Update()
	{
		Network::TransformMsg tdata;
		tdata.position = vec3(1.f, 2.f, 3.f);
		tdata.rotation = vec3(4.f, 5.f, 6.f);
		
		while (running_)
		{
			ProccesSdlEvent();

			Log("ServerMainLoop.");
			++tdata.id;
			tdata.position += vec3(0.25, 0.1, 0.2);

			{
				auto m = std::make_shared<Network::TransformMsg>(tdata);

				std::lock_guard<std::mutex> lk(usersMutex);
				for (auto& user : users)
					gateway_->AddToOutbox(user.first, m);
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(200));
		}
	}
	void GameServer::OnMessageArrived(const Network::BoxMessage & mesage)
	{
		std::lock_guard<std::mutex> lk(usersMutex);
		auto& user = users[mesage.first];
		user.Handle(mesage.second);

		auto characterId = user.GetUsageCharacterId();
		auto character	 = manager_.GetHero(characterId);

		if (character == nullptr)
			return;

		character->Handle(mesage);
	}

	void GameServer::NewUser(const std::string& name, uint32 id)
	{
		std::lock_guard<std::mutex> lk(usersMutex);
		users.insert({ id,  User(context_, name, id) });
	}

	void GameServer::ProccesSdlEvent()
	{
		SDL_Event event;
		if (SDL_PollEvent(&event) == 0)
			return;

		switch (event.type)
		{
		case SDL_QUIT: running_ = false; break;
		case SDL_KEYDOWN:			
			switch (event.key.keysym.sym)
			{
			case SDLK_ESCAPE: running_ = false; break;
			}
			break;
		}
	}
} // GameServer