#pragma once
#include "../IMessage.h"
#include "../MessageTarget.h"
#include "TransformMessageTypes.h"
#include "GLM/GLMUtils.h"

namespace Network
{
	struct TransformMsgReq : public IMessage
	{
		TransformMsgReq()
			: IMessage(MessageTypes::TransformReq, MessageTarget::All)
		{}
		TransformMessageTypes type = TransformMessageTypes::NONE;
		TransformAction action = TransformAction::POP;
		uint32 id = 0;

		virtual std::string ToString() override
		{
			return "ID: " + std::to_string(id) + "\nType: " + std::to_string(type) + "\nAction: " + std::to_string(action);
		}
	};
} // Network
