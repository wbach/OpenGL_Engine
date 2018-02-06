#pragma once
#include "NetworkCharacter.h"
#include <functional>
#include <memory>

namespace common
{
	namespace Hero
	{
		struct CommonStats;
	} // Hero
} // common

namespace GameEngine
{
	class InputManager;
	namespace Renderer
	{
		class RenderersManager;
	} // Renderer
} // GameEng

namespace MmmoRpg
{
	class ModelsCreator;

	class NetworkCharacterManager
	{
		typedef std::function<void (CGameObject*, const vec3&)> AddObject;
	public:
		NetworkCharacterManager(ModelsCreator* modelCreator, GameEngine::Renderer::RenderersManager& rendererManager, AddObject addObject);
		void AddCharacter(uint32 id, uint32 classId, const vec3& position, const common::Hero::CommonStats& stats);
		void Update(float deltaTime);
		void RemoveCharacter(uint32 id);

	private:
		std::unordered_map<uint32, std::shared_ptr<NetworkCharacter>> networkCharacters_;
		ModelsCreator* modelCreator_;
		GameEngine::Renderer::RenderersManager& rendererManager_;
		AddObject addObject_;
	};

	typedef std::unique_ptr<NetworkCharacterManager> NetworkCharacterManagerPtr;
} // MmmoRpg