#pragma once
#include "Types.h"
#include "GLM/GLMUtils.h"

namespace Network
{
	enum MessageTypes
	{
		TestDataMsg = 1,
		Authentication,
		ConnectionMsg
	};

	class IMessage
	{
	public:
		virtual ~IMessage() {}
		virtual std::string ToString() { return std::string(); }
		IMessage(uint8 type) 
			: msgType(type)
		{}
		uint8 GetType() { return msgType; }
	private:
		uint8 msgType;
	};


	struct MessageHeader
	{
		//uint16 payloadSize;
		uint8  msgType;
	};

	struct TestData : public IMessage
	{
		TestData() 
			: IMessage(MessageTypes::TestDataMsg)
		{}
		uint32 id = 0;
		uint32 type = 0;
		vec3 position;
		vec3 rotation;
		//std::string str;
		virtual std::string ToString() override
		{
			return "ID: " + std::to_string(id) + "\nType: " + std::to_string(type) + "\nPosition: " + Utils::ToString(position) + "\nRotation : " + Utils::ToString(rotation);
		}
	};

	enum ConnectionStatus
	{
		CONNECTED = 1,
		WAIT_FOR_AUTHENTICATION,
		ERROR_FULL,
		ERROR_FAILD_AUTHENTICATION
	};

	struct ConnectionMessage : public IMessage
	{
		ConnectionMessage()
			: IMessage(MessageTypes::ConnectionMsg)
		{}
		ConnectionMessage(ConnectionStatus status)
			: IMessage(MessageTypes::ConnectionMsg)
			, connectionStatus(status)
		{}
		uint8 connectionStatus = 0;

		virtual std::string ToString() override
		{
			return "ConnectionStatus: " + std::to_string(connectionStatus);
		}
	};

	const uint16 arraySize = 256;
	class AuthenticationMessage : public IMessage
	{
	public:
		AuthenticationMessage()
			: AuthenticationMessage("", "")
		{
		}
		AuthenticationMessage(const std::string& username, const std::string& password)
			: IMessage(MessageTypes::Authentication)
		{
			memset(userName_, 0, arraySize);
			memset(password_, 0, arraySize);

			CopyToArray(userName_, username);
			CopyToArray(password_, password);
		}

		std::string GetUserName() 
		{
			return userName_;
		}

		std::string GetPassword()
		{
			return password_;
		}

		virtual std::string ToString() override
		{
			return "Username: " + GetUserName() + "\nPassword: " + GetPassword();
		}

	private:
		void CopyToArray(char* dest, const std::string& str)
		{
			int i = 0;
			for (auto c : str)
			{
				if (i >= arraySize - 1)
					break;

				dest[i] = c;
				++i;
			}
		}

	private:		
		char userName_[arraySize];
		char password_[arraySize];
	};
}
