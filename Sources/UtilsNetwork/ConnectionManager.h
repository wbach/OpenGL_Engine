#pragma once
#include "Types.h"
#include "ConectContext.h"
#include "ISDLNetWrapper.h"
#include "Sender.h"
#include <unordered_map>
#include <functional>

namespace Network
{
	typedef std::function<void(uint32)> CreationFunc;

	class ConnectionManager
	{
	public:
		ConnectionManager(ISDLNetWrapper* sdlNetWrapper, ConectContext& context);
		void CheckNewConnectionsToServer();
		bool CheckSocketsActivity();
		void SubscribeForNewUser(CreationFunc func);
		void SubscribeForDisconnectUser(CreationFunc func);

	private:
		void WaitForAuthentication();
		bool IsSomthingOnServerSocket();
		bool ProccessAuthentication(Users::iterator& userIter);
		void CreateClientSocketIfAvailable();
		void DissmissConection();
		bool IsSpace() const;

	private:
		ConectContext& context_;
		uint32 clientsCount_;
		std::shared_ptr<ISDLNetWrapper> sdlNetWrapper_;
		Sender sender_;		
		Users notAuthenticatedUsers;
		std::vector<CreationFunc> newUserSubscribes_;

		// tmp simple dataBase
		std::unordered_map<std::string, std::string> usersDb_;
	};
}