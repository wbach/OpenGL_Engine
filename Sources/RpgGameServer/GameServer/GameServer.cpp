#include "GameServer.h"
#include <functional>
#include "Logger/Log.h"
#include "Database/DatabaseWrapperMock.h"
#include "Handlers/Distribute/DistributeHandler.h"
#include "Handlers/CharacterController/CharacterControllerHandler.h"
#include "Handlers/SelectCharacter/SelectCharacterHandler.h"
#include "Time/TimeMeasurer.h"

namespace GameServer
{
	GameServer::GameServer()
		: running_(true)
		, gateway_(new Network::CGateway())
		, context_(std::make_shared<Database::DatabaseWrapperMock>(), std::bind(&Network::CGateway::AddToOutbox, gateway_.get(), std::placeholders::_1, std::placeholders::_2))
	{		
		gateway_->StartServer(30, 1991);
		gateway_->SubscribeForNewUser(std::bind(&Context::NewUser, &context_, std::placeholders::_1, std::placeholders::_2));
		gateway_->SubscribeOnMessageArrived(std::bind(&GameServer::OnMessageArrived, this, std::placeholders::_1));

		Handler::IHandlerPtr distributeHandler(new Handler::DistributeHandler(context_));
		Handler::IHandlerPtr selectCharacterHandler(new Handler::SelectCharacterHandler(context_));
		Handler::IHandlerPtr characterControllerHandler(new Handler::CharacterControllerHandler(context_));
		
		dispatcher_.AddHandlers(
			{
			distributeHandler,
			selectCharacterHandler,
			characterControllerHandler
			});


		Update();
	}
	void GameServer::Update()
	{	
		Utils::Time::CTimeMeasurer timeMeasurer(60);;

		while (running_)
		{
			ProccesSdlEvent();
			context_.manager_.UpdateAllControllers(static_cast<float>(timeMeasurer.GetDeltaTime()));
			timeMeasurer.CalculateAndLock();
		}
	}

	void GameServer::OnMessageArrived(const Network::BoxMessage & mesage)
	{
		dispatcher_.Dispatch(mesage);
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