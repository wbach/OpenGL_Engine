#pragma once
#include "IDatabaseWrapper.h"
#include "Types.h"
#include "../Hero/BaseHero.h"
#include <vector>
#include <memory>
#include <unordered_map>

namespace GameServer
{
	namespace Database
	{
		class LocalDatabase
		{
		public:
			LocalDatabase(IDatabaseWrapperPtr dataBase);
			void Synchronize();

		private:
			CharactersMap allCharacters_;
			IDatabaseWrapperPtr dataBase_;
		};
	} // DataBase
} // GameServer