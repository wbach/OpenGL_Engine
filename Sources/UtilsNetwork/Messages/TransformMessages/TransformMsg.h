#pragma once
#include "../IMessage.h"
#include "../MessageTarget.h"
#include "TransformMessageTypes.h"
#include "GLM/GLMUtils.h"

namespace Network
{
	struct TransformMsg : public IMessage
	{
		TransformMsg()
			: IMessage(MessageTypes::Transform, MessageTarget::All)
		{}
		TransformMessageTypes type = TransformMessageTypes::NONE;
		uint32 id = 0;
		vec3 position;
		vec3 rotation;

		virtual std::string ToString() override
		{
			return "ID: " + std::to_string(id) + "\nType: " + std::to_string(type) + "\nPosition: " + Utils::ToString(position) + "\nRotation : " + Utils::ToString(rotation);
		}
	};
} // Network
