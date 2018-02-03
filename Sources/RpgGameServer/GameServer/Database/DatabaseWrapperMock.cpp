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
				data.name = allCharacters_[id]->GetName();
			}
			break;
			case 101:
			{
				data.mapId = 1;
				data.lvl = 12;
				data.classId = 1;
				data.name = allCharacters_[id]->GetName();
			}
			break;
			case 56:
			{
				data.mapId = 1;
				data.lvl = 151;
				data.classId = 1;
				data.name = allCharacters_[id]->GetName();
			}
			break;
			case 58:
			{
				data.mapId = 1;
				data.lvl = 2;
				data.classId = 1;
				data.name = allCharacters_[id]->GetName();
			}
			break;
			case 62:
			{
				data.mapId = 1;
				data.lvl = 6;
				data.classId = 1;
				data.name = allCharacters_[id]->GetName();
			}
			break;
			case 63:
			{
				data.mapId = 1;
				data.lvl = 80;
				data.classId = 1;
				data.name = allCharacters_[id]->GetName();
			}
			break;
			default:
				{
					data.mapId = 1;
					data.lvl = 1;
					data.classId = 1;
					data.name = "Mock : default";					
				}
			}
			return data;
		}

		wb::optional<common::Hero::Stats> DatabaseWrapperMock::GetCharacterStats(uint32 stats)
		{			
			return common::Hero::Stats();
		}

		wb::optional<DatabaseTransformInfo> DatabaseWrapperMock::GetTransformInfo(uint32 characterId)
		{
			switch (characterId)
			{
			case 98: return DatabaseTransformInfo{ 98, vec3(1.f, 0.f, 1.f), vec3(0.0f) }; // TO DO : more
			case 101: return DatabaseTransformInfo{ 98, vec3(2.f, 0.f, 1.f), vec3(0.0f) }; // TO DO : more
			case 56: return DatabaseTransformInfo{ 98, vec3(1.f, 0.f, 2.f), vec3(0.0f) }; // TO DO : more
			case 58: return DatabaseTransformInfo{ 98, vec3(2.f, 0.f, 2.f), vec3(0.0f) }; // TO DO : more
			case 62: return DatabaseTransformInfo{ 98, vec3(0.f, 0.f, 1.f), vec3(0.0f) }; // TO DO : more
			case 63: return DatabaseTransformInfo{ 98, vec3(2.f, 0.f, 0.f), vec3(0.0f) }; // TO DO : more

			}
			return wb::optional<DatabaseTransformInfo>();
		}

	} // Database
} // GameServer