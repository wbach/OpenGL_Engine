#include "GameServer.h"
#include <functional>
#include "Database/DatabaseWrapperMock.h"
#include "Handlers/CharacterController/CharacterControllerHandler.h"
#include "Handlers/Distribute/DistributeHandler.h"
#include "Handlers/GetCharacters/GetCharactersHandler.h"
#include "Handlers/GetCharactersData/GetCharactersDataHandler.h"
#include "Handlers/SelectCharacter/SelectCharacterHandler.h"
#include "Logger/Log.h"
#include "Time/TimeMeasurer.h"

namespace GameServer
{
GameServer::GameServer()
    : running_(true)
    , context_(std::make_shared<Database::DatabaseWrapperMock>(), std::bind(&GameServer::Send, this, std::placeholders::_1, std::placeholders::_2))
{
    gateway_->StartServer(30, 1991);
    gateway_->SubscribeForNewUser(std::bind(&Context::NewUser, &context_, std::placeholders::_1, std::placeholders::_2));
    gateway_->SubscribeForDisconnectUser(std::bind(&Context::DeleteUser, &context_, std::placeholders::_1));
    // gateway_->SubscribeOnMessageArrived(std::bind(&GameServer::OnMessageArrived, this, std::placeholders::_1));

    common::AbstractHandlerPtr distributeHandler(new Handler::DistributeHandler(context_));
    common::AbstractHandlerPtr getCharactersHandler(new Handler::GetCharactersHandler(context_));
    common::AbstractHandlerPtr selectCharacterHandler(new Handler::SelectCharacterHandler(context_));
    common::AbstractHandlerPtr characterControllerHandler(new Handler::CharacterControllerHandler(context_));
    common::AbstractHandlerPtr getCharacterDataHandler(new Handler::GetCharactersDataHandler(context_));

    dispatcher_.AddHandlers({// distributeHandler,
                             {"distributeHandler", selectCharacterHandler},
                             {"getCharactersHandler", characterControllerHandler},
                             {"selectCharacterHandler", getCharactersHandler},
                             {"getCharacterDataHandler", getCharacterDataHandler}});

    gateway_->SubscribeOnMessageArrived("Dispat", std::bind(&GameServer::OnMessageArrived, this, std::placeholders::_1));

    Update();
}

void GameServer::Update()
{
    Utils::Time::CTimeMeasurer timeMeasurer(120, false);

    while (running_)
    {
        timeMeasurer.StartFrame();
        gateway_->Update();
        ProccesSdlEvent();
        context_.manager_.UpdateAllControllers(static_cast<float>(timeMeasurer.GetDeltaTime()));
        timeMeasurer.EndFrame();
    }
}

void GameServer::OnMessageArrived(std::unique_ptr<Network::IMessage> mesage)
{
    dispatcher_.Dispatch(*mesage);
}

void GameServer::Send(uint32 id, Network::IMessage* ptr)
{
    gateway_->Send(id, ptr);
}

void GameServer::ProccesSdlEvent()
{
    SDL_Event event;
    if (SDL_PollEvent(&event) == 0)
        return;

    switch (event.type)
    {
        case SDL_QUIT:
            running_ = false;
            break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {
                case SDLK_ESCAPE:
                    running_ = false;
                    break;
            }
            break;
    }
}
}  // namespace GameServer
