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
			virtual CharacterInDataBase GetCharacterData(uint32 id) override;
			virtual ~DatabaseWrapperMock() override {}
		};
	} // Database
} // GameServer