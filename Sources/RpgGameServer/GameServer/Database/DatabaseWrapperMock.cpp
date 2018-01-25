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

		wb::optional<Hero::CharacterData> DatabaseWrapperMock::GetCharacterData(uint32 id)
		{
			Hero::CharacterData data;

			switch (id)
			{
			case 98:
			{
				data.mapId = 1;
				data.lvl = 12;
				data.classId = 1;
				data.name = "BaszekBK";
			}
			break;
			case 101:
			{
				data.mapId = 1;
				data.lvl = 12;
				data.classId = 1;
				data.name = "Super-Knight";
			}
			break;
			case 56:
			{
				data.mapId = 1;
				data.lvl = 151;
				data.classId = 1;
				data.name = "Black Knight";
			}
			break;
			default:
				{					
					data.mapId = 1;
					data.lvl = 1;
					data.classId = 1;
					data.name = "BaszekBK";					
				}
			}
			return data;
		}

		wb::optional<Hero::Stats> DatabaseWrapperMock::GetCharacterStats(uint32 stats)
		{			
			return Hero::Stats();
		}

	} // Database
} // GameServer