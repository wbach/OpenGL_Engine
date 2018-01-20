#include "Manager.h"
#include "Hero/HeroFactory.hpp"
#include "ControllerCreator.h"

namespace GameServer
{
	Hero::BaseHeroPtr Manager::GetHero(uint32 id)
	{
		for (auto& m : maps_)
		{
			auto character = m.GetCharacter(id);
			if (character != nullptr)
				return character;
		}
		return nullptr;
	}
	void Manager::AddHero(uint32 id)
	{
		auto characterData = databaseWrapper_->GetCharacterData(id);

		auto knight = HeroFactory::CreateKnight();

		knight->context_.mapId = characterData.mapId;
		knight->context_.stats_ = characterData.stats;
		CreateCharacterController(knight, Controllers::CharacterControllerType);

		maps_[characterData.mapId].AddCharacter(id, knight);
	}
	void Manager::UpdateAllControllers(float dt)
	{
		for (auto& map : maps_)
			map.UpdateAllControllers(dt);
	}
} // GameServer
