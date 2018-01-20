#include "LocalDatabase.h"

namespace GameServer
{
	namespace Database
	{
		LocalDatabase::LocalDatabase(std::shared_ptr<IDatabaseWrapper> dataBase)
			: dataBase_(dataBase)
		{
		}

		void LocalDatabase::Synchronize()
		{
		}

	} // DataBase
} // GameServer