#pragma once
#include "Database/IDatabaseWrapper.h"
#include "Messages/IMessage.h"
#include "User.h"
#include "Manager.h"
#include "Mutex.hpp"
#include <functional>

namespace GameServer
{
	typedef std::function<void(uint32, Network::IMessage*)> SendMessage;

	class Context
	{
	public:
		Context(Database::IDatabaseWrapperPtr databaseWrapperPtr, SendMessage sendMessage)
			: databaseWrapper_(databaseWrapperPtr)
			, sendMessage_(sendMessage)
			, manager_(databaseWrapper_)
		{}

		void NewUser(const std::string& name, uint32 id);
		void DeleteUser(uint32 id);
		const UsersMap& GetUsers();
		User& GetUser(uint32 id) { return users_[id]; }

	public:
		Database::IDatabaseWrapperPtr databaseWrapper_;
		SendMessage sendMessage_;
		Manager		manager_;

	private:
		std::mutex usersMutex;
		UsersMap   users_;
	};
} // GameServer
