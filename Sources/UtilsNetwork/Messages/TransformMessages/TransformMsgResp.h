#pragma once
#include "../IMessage.h"
#include "../MessageTarget.h"
#include "TransformMessageTypes.h"
#include "GLM/GLMUtils.h"

namespace Network
{
	struct TransformMsgResp : public IMessage
	{
		TransformMsgResp()
			: IMessage(MessageTypes::TransformResp, MessageTarget::All)
		{}
		TransformMessageTypes type = TransformMessageTypes::NONE;
		TransformAction action = TransformAction::POP;
		uint32 id = 0;
		vec3 position;
		vec3 rotation;

		virtual std::string ToString() override
		{
			return "ID: " + std::to_string(id) + "\nType: " + std::to_string(type) + "\nAction: " + std::to_string(action) + "\nPosition: " + Utils::ToString(position) + "\nRotation : " + Utils::ToString(rotation);
		}
	};
} // Network
