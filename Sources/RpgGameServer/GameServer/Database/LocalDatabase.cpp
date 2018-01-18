#include "LocalDatabase.h"

namespace GameServer
{
	namespace Database
	{
		LocalDatabase::LocalDatabase(IDatabaseWrapperPtr dataBase)
			: dataBase_(dataBase)
		{
		}

		void LocalDatabase::Synchronize()
		{
		}

	} // DataBase
} // GameServer