#pragma once
#include "Types.h"
#include "ConectContext.h"
#include "ISDLNetWrapper.h"
#include "Sender.h"
#include "Reciever.h"
#include <unordered_map>
#include <functional>

namespace Network
{
	typedef std::function<void(const std::string&, uint32)> CreationFunc;

	class ConnectionManager
	{
	public:
		ConnectionManager(ISDLNetWrapperPtr sdlNetWrapper, ConectContext& context);
		void CheckNewConnectionsToServer();
		bool CheckSocketsActivity();
		void SubscribeForNewUser(CreationFunc func);
		void SubscribeForDisconnectUser(CreationFunc func);
		void DisconectUser(uint32 id);

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
		ISDLNetWrapperPtr sdlNetWrapper_;
		Sender sender_;
		Receiver receiver_;
		Users notAuthenticatedUsers;
		std::vector<CreationFunc> newUserSubscribes_;

		// tmp simple dataBase
		std::unordered_map<std::string, std::string> usersDb_;
	};
}