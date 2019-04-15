#include "CharacterControllerHandler.h"
#include "Common/Controllers/CharacterController/Character.h"
#include "Common/Controllers/CharacterController/NetworkActionsConverter.h"
#include "Messages/TransformMessages/TransformMsgReq.h"
#include "Messages/TransformMessages/TransformMsgResp.h"

using namespace common::Controllers;

namespace GameServer
{
	namespace Handler
	{
		CharacterControllerHandler::CharacterControllerHandler(Context& context)
			: common::AbstractHandler(Network::MessageTypes::TransformReq)
			, context_(context)
		{}

		void CharacterControllerHandler::ProcessMessage(const Network::BoxMessage & message)
		{
			auto msg = Network::castMessageAs<Network::TransformMsgReq>(message.second.get());

			if (msg == nullptr)
				return;

			auto characterId = context_.GetUser(message.first).GetUsageCharacterId();

			if (!characterId)
			{
				ERROR_LOG("CharacterControllerHandler::ProcessMessage receive message for not user who didnt select character yet.");
				return;
			}

			auto hero = context_.manager_.GetHero(characterId.value());

			if (hero == nullptr)
			{
				ERROR_LOG("CharacterControllerHandler::ProcessMessage cant get character by id.");
				return;
			}

			auto controller = hero->GetControllerByType(CharacterControllerType);

			if (controller == nullptr)
			{
				ERROR_LOG("CharacterControllerHandler::ProcessMessage character controller not found in character.");
				return;
			}

			auto characterController = common::Controllers::castControllerAs<CharacterController>(controller);
			
			switch (msg->action)
			{				
			case Network::TransformAction::PUSH:
				characterController->AddState(NetworkActionsConverter::Convert(msg->type));
				break;
			case Network::TransformAction::POP:
				characterController->RemoveState(NetworkActionsConverter::Convert(msg->type));
				break;
			}

			auto tdata = std::make_unique < Network::TransformMsgResp> ();
			tdata->position = characterController->GetTransform().GetPosition();
			tdata->rotation = characterController->GetTransform().GetRotation();
			tdata->id = msg->id;
			tdata->type = msg->type;
			tdata->action = msg->action;

			for (auto& user : context_.GetUsers())
			{
				context_.sendMessage_(user.first, tdata.get());
			}
		}
	} // Handler
} // GameServer