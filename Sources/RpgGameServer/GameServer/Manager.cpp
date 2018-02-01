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
		auto characterStats = databaseWrapper_->GetCharacterStats(id);

		if (!characterData)
			return;

		auto knight = HeroFactory::CreateKnight();

		knight->context_.data_.mapId = characterData.value().mapId;
		knight->context_.stats_ = characterStats.value();
		CreateCharacterController(knight, common::Controllers::CharacterControllerType);

		maps_[characterData.value().mapId].AddCharacter(id, knight);
	}
	void Manager::UpdateAllControllers(float dt)
	{
		for (auto& map : maps_)
			map.UpdateAllControllers(dt);
	}
} // GameServer
