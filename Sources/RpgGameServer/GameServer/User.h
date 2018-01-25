#pragma once
#include "Types.h"
#include <vector>
#include <list>
#include "optional.hpp"
#include <unordered_map>

namespace GameServer
{
	enum UserState
	{
		CharacterSelect,
		CreateNewCharacter,
		InGame
	};

	typedef std::vector<uint32> Characters;

	class User
	{
	public:
		User() {}
		User(const std::string& name, uint32 id);
		uint32 GetId();
		bool HasCharacter(uint32 id);
		const Characters& GetCharacters();
		void SetUsageCharacter(uint32);
		void SetCharacters(Characters characters);
		wb::optional<uint32> GetUsageCharacterId();

	private:
		uint32 id_;
		std::string name_;
		
		UserState state_;
		Characters  characters_;

		wb::optional<uint32>  usageCharacterId_;
	};

	typedef std::unordered_map<uint32, User> UsersMap;

} // GameServer