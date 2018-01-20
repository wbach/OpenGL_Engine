#pragma once
#include "Types.h"
#include "Types/Character.h"
#include "../Hero/BaseHero.h"
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
			virtual CharacterInDataBase GetCharacterData(uint32 id) = 0;

		protected:
			CharactersMap allCharacters_;
		};

		typedef std::shared_ptr<IDatabaseWrapper> IDatabaseWrapperPtr;
	} // DataBase
} // GameServer