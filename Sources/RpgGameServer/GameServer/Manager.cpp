#include "Manager.h"
#include "Hero/HeroFactory.hpp"
#include "ControllerCreator.h"
#include "Logger/Log.h"

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
		auto characterTransformInfo = databaseWrapper_->GetTransformInfo(id);

		if (!characterData)
			return;

		auto knight = HeroFactory::CreateKnight();

		knight->context_.data_.mapId = characterData.value().mapId;
		knight->context_.stats_ = characterStats.value();

		if (characterTransformInfo)
		{
			knight->context_.transform_.SetPosition(characterTransformInfo.value().position);
			knight->context_.transform_.SetRotation(characterTransformInfo.value().rotation);
		}
		CreateCharacterController(knight);

		maps_[characterData.value().mapId].AddCharacter(id, knight);
	}
	void Manager::RemoveHero(uint32 id)
	{
		for (auto& m : maps_)
		{
			auto character = m.GetCharacter(id);
			if (character != nullptr)
			{
				m.RemoveCharacter(id);
				return;
			}
		}
	}
	void Manager::UpdateAllControllers(float dt)
	{
		for (auto& map : maps_)
			map.UpdateAllControllers(dt);
	}
	const CharactersMap& Manager::GetAllCharactersInMap(uint32 mapId) const
	{
		if (mapId >= maps_.size())
		{
			Log("anager::GetAllCharactersInMap: map id : "+ std::to_string(mapId) + " not found.");
			return emptyCharactersMap_;
		}
		return maps_[mapId].GetAllCharacters();
	}
} // GameServer
