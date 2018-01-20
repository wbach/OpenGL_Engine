#include "User.h"
#include "Logger/Log.h"
#include "Database/IDatabaseWrapper.h"

namespace GameServer
{
	User::User(const std::string & name, uint32 id)
		: id_(id)
		, name_(name)
	{

	}
	uint32 User::GetId()
	{
		return id_;
	}
	void User::SetCharacters(Characters characters)
	{
		characters_ = characters;
	}
	wb::optional<uint32> User::GetUsageCharacterId()
	{
		return usageCharacterId_;
	}
	bool User::HasCharacter(uint32 id)
	{
		for (auto c : characters_)
		{
			if (c == id)
				return true;
		}
		return false;
	}
	void User::SetUsageCharacter(uint32 id)
	{
		usageCharacterId_ = id;
	}
} // GameServer