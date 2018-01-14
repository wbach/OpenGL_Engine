#pragma once
#include "Types.h"
#include "ConectContext.h"
#include "ISDLNetWrapper.h"
#include "Sender.h"
#include <unordered_map>

namespace Network
{
	class ConnectionManager
	{
	public:
		ConnectionManager(ISDLNetWrapper* sdlNetWrapper, ConectContext& context);
		void CheckNewConnectionsToServer();
		bool CheckSocketsActivity();

	private:
		void WaitForAuthentication();
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


		// tmp simple dataBase
		std::unordered_map<std::string, std::string> usersDb_;
	};
}