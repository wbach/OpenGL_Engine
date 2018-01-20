#include "CharacterControllerHandler.h"
#include "../../../Common/Controllers/Character.h"
#include "Messages/TransformMessages/TransformMsg.h"

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

			auto controller = hero->GetControllerByType(Controllers::CharacterControllerType);

			if (controller == nullptr)
			{
				Log("CharacterControllerHandler::ProcessMessage character controller not found in character.");
				return;
			}

			auto characterController = dynamic_cast<Controllers::CharacterController*>(controller.get());

			auto msg = dynamic_cast<Network::TransformMsg*>(message.second.get());

			if (msg == nullptr)
				return;

			switch (msg->type)
			{
			case Network::TransformMessageTypes::MOVE_FORWARD:	characterController->AddState(Controllers::CharacterActions::MOVE_FORWARD); break;
			case Network::TransformMessageTypes::MOVE_BACKWARD:	characterController->AddState(Controllers::CharacterActions::MOVE_BACKWARD); break;
			case Network::TransformMessageTypes::ROTATE_RIGHT:	characterController->AddState(Controllers::CharacterActions::ROTATE_RIGHT); break;
			case Network::TransformMessageTypes::ROTATE_LEFT:	characterController->AddState(Controllers::CharacterActions::ROTATE_LEFT); break;
			case Network::TransformMessageTypes::JUMP:			/*AddState(CharacterActions::JUMP);*/ break;
			}		


		}
	} // Handler
} // GameServer