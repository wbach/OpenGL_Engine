#pragma once
#include "IDatabaseWrapper.h"

namespace GameServer
{
	namespace Database
	{
		class DatabaseWrapperMock : public IDatabaseWrapper
		{
		public:
			DatabaseWrapperMock();
			virtual UserCharacterVec GetCharacterByUser(uint32 id) override;
			virtual Hero::BaseHeroPtr GetCharacter(uint32 id) override;

		private:
			
		};
	} // Database
} // GameServer