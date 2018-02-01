#include "CharacterControllerHandler.h"
#include "../../../../Common/Controllers/CharacterController/Character.h"
#include "Messages/TransformMessages/TransformMsg.h"

using namespace common::Controllers;

namespace GameServer
{
	namespace Handler
	{
		CharacterControllerHandler::CharacterControllerHandler(Context & context)
			: IHandler(Network::MessageTypes::Transform)
			, context_(context)
		{}

		void CharacterControllerHandler::ProcessMessage(const Network::BoxMessage & message)
		{
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

			auto characterController = dynamic_cast<CharacterController*>(controller.get());

			auto msg = dynamic_cast<Network::TransformMsg*>(message.second.get());

			if (msg == nullptr)
				return;

			switch (msg->type)
			{
			case Network::TransformMessageTypes::MOVE_FORWARD:	characterController->AddState(CharacterActions::MOVE_FORWARD); break;
			case Network::TransformMessageTypes::MOVE_BACKWARD:	characterController->AddState(CharacterActions::MOVE_BACKWARD); break;
			case Network::TransformMessageTypes::ROTATE_RIGHT:	characterController->AddState(CharacterActions::ROTATE_RIGHT); break;
			case Network::TransformMessageTypes::ROTATE_LEFT:	characterController->AddState(CharacterActions::ROTATE_LEFT); break;
			case Network::TransformMessageTypes::JUMP:			/*AddState(CharacterActions::JUMP);*/ break;
			}		

			Network::TransformMsg tdata;
			tdata.position = characterController->GetTransform().GetPosition();
			tdata.rotation = characterController->GetTransform().GetRotation();
			tdata.id = msg->id;
			tdata.type = msg->type;
			auto m = std::make_shared<Network::TransformMsg>(tdata);

			SendToAll(m);
		}

		void CharacterControllerHandler::SendToAll(const Network::IMessagePtr& message)
		{
			for (auto& user : context_.GetUsers())
			{
				context_.sendMessage_(user.first, message);
			}
		}
	} // Handler
} // GameServer