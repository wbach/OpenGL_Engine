#include "WaitForConfirmationState.h"
#include "Common/MessageHandling/Dispatcher.h"
#include "TestGame/MRpg/MrpgGameContext.h"
#include "TestGame/MRpg/Handlers/SelectCharacter/SelectCharacterHandler.h"
#include "UtilsNetwork/Messages/SelectCharacter/SelectCharacterMsgReq.h"
#include "UtilsNetwork/Gateway.h"

namespace MmmoRpg
{
	WaitForConfirmationState::WaitForConfirmationState(common::Dispacher& dispatcher, Network::CGateway& gateway, MrpgGameContext& gameContext, GameEngine::AddEvent& addEvent, std::vector<CharacterSlot>& charactersData)
		: dispatcher_(dispatcher)
		, gateway_(gateway)
		, gameContext_(gameContext)
		, charactersData_(charactersData)
		, addSceneEvent(addEvent)
	{
		dispatcher_.AddHandler("WaitForSelectResponseState", new SelectCharacterHandler(std::bind(&WaitForConfirmationState::SceneToLoad, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		SendSelectCharacterReq();
	}
	WaitForConfirmationState::~WaitForConfirmationState()
	{
		dispatcher_.RemoveHandler("WaitForSelectResponseState");
	}
	void WaitForConfirmationState::SendSelectCharacterReq()
	{
		auto characterSelectReq = std::make_unique<Network::SelectCharacterMsgReq>();
		characterSelectReq->id = charactersData_[gameContext_.selectedCharacterId].characterInfo.id_;
		gateway_.Send(characterSelectReq.get());
		sentTime_ = std::chrono::high_resolution_clock::now();
		Log("SelectCharacterScene::SendSelectCharacterReq : Character selected");
		status_ = StateStatus::DONE;
	}
	void WaitForConfirmationState::Update()
	{
	}
	void WaitForConfirmationState::SceneToLoad(uint32 characterId, uint32 mapId, bool success)
	{
		if (!success)
		{
			status_ = StateStatus::BACK_TO_PREVIOUS_STATE;
			return;
		}

		std::string sceneName = "MainRpgScene";

		switch (mapId)
		{
		case 1: sceneName = "MainRpgScene";
			break;
		}

		gameContext_.selectedCharacterId = characterId;
		GameEngine::SceneEvent e(GameEngine::SceneEventType::LOAD_SCENE_BY_NAME, sceneName);
		addSceneEvent(e);
		status_ = StateStatus::DONE;
		Log("WaitForSelectResponseState load :" + sceneName);
	}
} // MmmoRpg