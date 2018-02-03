#include "CharacterControllerHandler.h"
#include "../../../../Common/Controllers/CharacterController/Character.h"
#include "../../../../Common/Controllers/CharacterController/NetworkActionsConverter.h"
#include "Messages/TransformMessages/TransformMsgReq.h"
#include "Messages/TransformMessages/TransformMsgResp.h"

using namespace common::Controllers;

namespace GameServer
{
	namespace Handler
	{
		CharacterControllerHandler::CharacterControllerHandler(Context& context)
			: IHandler(Network::MessageTypes::TransformReq)
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
				Log("CharacterControllerHandler::ProcessMessage receive message for not user who didnt select character yet.");
				return;
			}

			auto hero = context_.manager_.GetHero(characterId.value());

			if (hero == nullptr)
			{
				Log("CharacterControllerHandler::ProcessMessage cant get character by id.");
				return;
			}

			auto controller = hero->GetControllerByType(CharacterControllerType);

			if (controller == nullptr)
			{
				Log("CharacterControllerHandler::ProcessMessage character controller not found in character.");
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

			Network::TransformMsgResp tdata;
			tdata.position = characterController->GetTransform().GetPosition();
			tdata.rotation = characterController->GetTransform().GetRotation();
			tdata.id = msg->id;
			tdata.type = msg->type;
			tdata.action = msg->action;

			auto m = Network::CreateIMessagePtr<Network::TransformMsgResp>(tdata);
			for (auto& user : context_.GetUsers())
			{
				context_.sendMessage_(user.first, m);
			}
		}
	} // Handler
} // GameServer