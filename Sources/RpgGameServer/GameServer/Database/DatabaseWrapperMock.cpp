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
				{ 58,  std::make_shared<Hero::Knight>("Baszek2 BK") },
				{ 62,  std::make_shared<Hero::Knight>("Baszek2 DK") },
				{ 63,  std::make_shared<Hero::Knight>("Baszek2 BBK") },
				});
		}
		UserCharacterVec DatabaseWrapperMock::GetCharacterByUser(uint32 id)
		{
			if (id == 14)
				return { 98, 101, 56 };
			
			if( id == 7)
				return { 58, 62, 63 };

			return {};
		}

		wb::optional<common::Hero::CharacterData> DatabaseWrapperMock::GetCharacterData(uint32 id)
		{
			common::Hero::CharacterData data;

			switch (id)
			{
			case 98:
			{
				data.mapId = 1;
				data.lvl = 24;
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

		wb::optional<common::Hero::Stats> DatabaseWrapperMock::GetCharacterStats(uint32 stats)
		{			
			return common::Hero::Stats();
		}

	} // Database
} // GameServer