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
			LocalDatabase(std::shared_ptr<IDatabaseWrapper> dataBase);
			void Synchronize();

		private:
			CharactersMap allCharacters_;
			std::shared_ptr<IDatabaseWrapper> dataBase_;
		};
	} // DataBase
} // GameServer