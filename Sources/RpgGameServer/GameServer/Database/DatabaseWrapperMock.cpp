#include "DatabaseWrapperMock.h"
#include "../Hero/Classes/Knight.h"


namespace GameServer
{
	namespace Database
	{
		DatabaseWrapperMock::DatabaseWrapperMock()			
		{
			allCharacters_ = CharactersMap({
				{ 98,  std::make_shared<Hero::Knight>("BaszekBK") },
				{ 101, std::make_shared<Hero::Knight>("Super-Knight") },
				{ 56,  std::make_shared<Hero::Knight>("Black Knight") },
				});
		}
		UserCharacterVec DatabaseWrapperMock::GetCharacterByUser(uint32 id)
		{
			return {98, 101, 56};
		}

		CharacterInDataBase DatabaseWrapperMock::GetCharacterData(uint32 id)
		{
			CharacterInDataBase data;
			data.mapId = 1;
			return data;
		}

	} // Database
} // GameServer