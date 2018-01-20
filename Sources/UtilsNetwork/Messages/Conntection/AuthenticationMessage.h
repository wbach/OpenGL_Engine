#pragma once
#include "../IMessage.h"
#include "../MessageTarget.h"
#include "ConnectionStatus.h"

namespace Network
{
	class AuthenticationMessage : public IMessage
	{
	public:
		AuthenticationMessage()
			: AuthenticationMessage("", "")
		{
		}
		AuthenticationMessage(const std::string& username, const std::string& password)
			: IMessage(MessageTypes::Authentication, MessageTarget::Dedicated)
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
		char userName_[arraySize];
		char password_[arraySize];
	};
} // Network
