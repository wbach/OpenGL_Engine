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
			virtual wb::optional<Hero::CharacterData> GetCharacterData(uint32 id) override;
			virtual wb::optional<Hero::Stats> GetCharacterStats(uint32 stats) override;
			virtual ~DatabaseWrapperMock() override {}
		};
	} // Database
} // GameServer