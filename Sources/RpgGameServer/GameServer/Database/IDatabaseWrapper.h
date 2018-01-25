#pragma once
#include "Types.h"
#include "optional.hpp"
#include "../Hero/CharacterData.h"
#include "../Hero/BaseHero.h"
#include "../Hero/Stats.h"
#include <vector>
#include <memory>
#include <unordered_map>

namespace GameServer
{
	namespace Database
	{
		typedef std::vector<uint32> UserCharacterVec;
		typedef std::unordered_map<uint32, Hero::BaseHeroPtr> CharactersMap;

		class IDatabaseWrapper
		{
		public:
			virtual ~IDatabaseWrapper() {}
			virtual UserCharacterVec  GetCharacterByUser(uint32 id) = 0;
			virtual wb::optional<Hero::CharacterData> GetCharacterData(uint32 id) = 0;
			virtual wb::optional<Hero::Stats> GetCharacterStats(uint32 stats) = 0;

		protected:
			CharactersMap allCharacters_;
		};

		typedef std::shared_ptr<IDatabaseWrapper> IDatabaseWrapperPtr;
	} // DataBase
} // GameServer