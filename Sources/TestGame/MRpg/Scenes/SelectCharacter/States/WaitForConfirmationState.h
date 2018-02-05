#pragma once
#include "ISelectState.h"
#include "GameEngine/Scene/SceneEvents.h"
#include "TestGame/MRpg/Scenes/SelectCharacter/CharacterSlot.h"

namespace GameEngine
{
	namespace Renderer
	{
		class RenderersManager;
	} // Renderer
} // GameEngine

namespace Network
{
	class CGateway;
} // Network

namespace common
{
	class Dispacher;
} // common

namespace MmmoRpg
{
	struct MrpgGameContext;

	class WaitForConfirmationState : public ISelectState
	{
	public:
		WaitForConfirmationState(common::Dispacher& dispatcher, Network::CGateway& gateway, MrpgGameContext& gameContext, GameEngine::AddEvent& addEvent, std::vector<CharacterSlot>& charactersData);
		virtual ~WaitForConfirmationState() override;
		virtual void Update() override;

	private:
		void SceneToLoad(uint32 characterId, uint32 mapId, bool success);
		void SendSelectCharacterReq();

	private:
		common::Dispacher& dispatcher_;
		Network::CGateway& gateway_;
		MrpgGameContext& gameContext_;
		std::vector<CharacterSlot>& charactersData_;
		GameEngine::AddEvent addSceneEvent;
		Timepoint sentTime_;
	};
}