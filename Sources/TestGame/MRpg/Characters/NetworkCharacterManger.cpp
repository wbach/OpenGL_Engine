#include "NetworkCharacterManger.h"
#include "ModelsCreator.h"
#include "GameEngine/Renderers/RenderersManager.h"

namespace MmmoRpg
{
	NetworkCharacterManager::NetworkCharacterManager(ModelsCreator* modelCreator, GameEngine::Renderer::RenderersManager& rendererManager, AddObject addObject)
		: modelCreator_(modelCreator)
		, rendererManager_(rendererManager)
		, addObject_(addObject)
	{
	}

	void NetworkCharacterManager::AddCharacter(uint32 id, uint32 classId, const vec3& position, const common::Hero::CommonStats& stats)
	{
		if (networkCharacters_.count(id) > 0)
			return;

		auto modelWrapper = modelCreator_->CreateHero(classId);
		networkCharacters_[id] = std::make_shared<NetworkCharacter>(id, stats, modelWrapper);
		auto entity = networkCharacters_[id]->GetEntity();
		entity->dynamic = true;
		addObject_(entity, position);
		rendererManager_.Subscribe(entity);
	}
	void NetworkCharacterManager::Update(float deltaTime)
	{
		for (auto& character : networkCharacters_)
		{
			character.second->UpdateControllers(deltaTime);
		}
	}
	void NetworkCharacterManager::RemoveCharacter(uint32 id)
	{
		if (networkCharacters_.count(id) == 0)
			return;
		rendererManager_.UnSubscribe(networkCharacters_[id]->GetEntity());
		networkCharacters_.erase(id);
	}
} // MmmoRpg