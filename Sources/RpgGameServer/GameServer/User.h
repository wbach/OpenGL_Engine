#pragma once
#include "Types.h"
#include "Message.h"
#include "Context.h"
#include <vector>
#include <list>

namespace GameServer
{
	namespace Database
	{
		class IDatabaseWrapper;
	}

	enum UserState
	{
		CharacterSelect,
		CreateNewCharacter,
		InGame
	};

	class User
	{
	public:
		User() {}
		User(Context, const std::string& name, uint32 id);
		uint32 GetId();
		uint32 GetUsageCharacterId();
		Network::IMessagePtr Handle(const Network::IMessagePtr);

	private:
		uint32 id_;
		std::string name_;		
		
		uint32 usageCharacterId_;
		std::vector<uint32> characters_;

		std::list<UserState> state_;
		Context context_;
	};
} // GameServer