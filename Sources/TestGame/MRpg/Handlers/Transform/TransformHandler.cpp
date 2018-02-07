#include "TransformHandler.h"
#include "TestGame/MRpg/Characters/NetworkCharacterManger.h"
#include "Messages/TransformMessages/TransformMsgResp.h"
#include "Common/Controllers/CharacterController/Character.h"
#include "Common/Controllers/CharacterController/NetworkActionsConverter.h"

using namespace common::Controllers;

namespace MmmoRpg
{
	void TransformHandler::ProcessMessage(const Network::BoxMessage & message)
	{
		auto msg = Network::castMessageAs<Network::TransformMsgResp>(message.second.get());

		if (msg == nullptr)
		{
			Log("Got msg but wrong type : " + std::to_string(msg->GetType()));
			return;
		}
		HandleTransformMsg(*msg);
	}

	void TransformHandler::HandleTransformMsg(const Network::TransformMsgResp& msg)
	{
		auto characterId = msg.id;
		auto networkCharacter = networkCharacterManager_.GetCharacter(msg.id);

		if (networkCharacter == nullptr)
			return;
		
		auto entity = networkCharacterManager_.GetCharacter(msg.id)->GetEntity();

		if (entity == nullptr)
			return;

		entity->worldTransform.SetPosition(msg.position);
		entity->worldTransform.SetRotation(msg.rotation);

		auto icontroller = networkCharacter->GetControllerByType(CharacterControllerType);
		auto controller = castControllerAs<CharacterController>(icontroller);

		switch (msg.action)
		{
		case Network::TransformAction::PUSH:
			controller->AddState(NetworkActionsConverter::Convert(msg.type));
			break;
		case Network::TransformAction::POP:
			controller->RemoveState(NetworkActionsConverter::Convert(msg.type));
			break;
		}
	}
} // MmmoRpg