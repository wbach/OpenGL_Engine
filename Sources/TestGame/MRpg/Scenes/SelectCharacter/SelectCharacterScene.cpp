#include "SelectCharacterScene.h"
#include "../UtilsNetwork/Gateway.h"
#include "../UtilsNetwork/Messages/SelectCharacter/SelectCharacterMsgReq.h"
#include "../UtilsNetwork/Messages/SelectCharacter/SelectCharacterMsgResp.h"
#include "../UtilsNetwork/Messages/GetCharacters/GetCharactersMsgReq.h"
#include "../UtilsNetwork/Messages/GetCharacters/GetCharactersMsgResp.h"
#include "../GameEngine/Engine/Engine.h"
#include "../GameEngine/Renderers/GUI/Texutre/GuiTextureElement.h"
#include "../GameEngine/Objects/ObjectBuilder.h"

namespace MmmoRpg
{
	SelectCharacterScene::SelectCharacterScene(Network::CGateway& gateway, MrpgGameContext& gameContext)
		: CScene("SelectCharacterScene")
		, gateway_(gateway)
		, gameContext_(gameContext)
		, state_(State::GET_CHARACTER)
		, itemsTextColour_(0, 162.f / 255.f, 232.f / 255.f)
	{
		characterSelectText_.text = "Select Character";
		characterSelectText_.colour = itemsTextColour_;
		characterSelectText_.position = glm::vec2(-0.25, 0.5);
	}
	SelectCharacterScene::~SelectCharacterScene()
	{

	}
	int SelectCharacterScene::Initialize()
	{
		GameEngine::Renderer::Gui::GuiTextureElement guiTexture;
		guiTexture.texture = resourceManager.GetTextureLaoder().LoadTexture("GUI/character_select.jpg", false);
		renderersManager_->GuiTexture("bg") = guiTexture;

		renderersManager_->GuiText("SelectCharacter") = characterSelectText_;


		//camera->SetPosition(vec3());
		//CGameObject* obj[5];
		//CGameObject* addobj[5];
		//obj[0] = ObjectBuilder::CreateEntity(resourceManager, glm::vec3(0, 0.5, 0), "Meshes/DaeAnimationExample/CharacterRunning.dae");
		//addobj[0] = AddGameObject(obj[0], glm::vec3(-3, -3, -5));
		//renderersManager_->Subscribe(addobj[0]);

		//obj[1] = ObjectBuilder::CreateEntity(resourceManager, glm::vec3(0, 0.5, 0), "Meshes/DaeAnimationExample/CharacterRunning.dae");
		//addobj[1] = AddGameObject(obj[1], glm::vec3(-1.5, -3, -5));
		//renderersManager_->Subscribe(addobj[1]);

		//obj[2] = ObjectBuilder::CreateEntity(resourceManager, glm::vec3(0, 0.5, 0), "Meshes/DaeAnimationExample/CharacterRunning.dae");
		//addobj[2] = AddGameObject(obj[2], glm::vec3(0.5, -3, -5));
		//renderersManager_->Subscribe(addobj[2]);

		//obj[3] = ObjectBuilder::CreateEntity(resourceManager, glm::vec3(0, 0.5, 0), "Meshes/DaeAnimationExample/CharacterRunning.dae");
		//addobj[3] = AddGameObject(obj[3], glm::vec3(2.5, -3, -5));
		//renderersManager_->Subscribe(addobj[3]);


	//	return 0;

		SendGetCharacter();
		WaitForGetCharacterResp();

		return 0;
	}
	void SelectCharacterScene::PostInitialize()
	{
	}
	int SelectCharacterScene::Update(float deltaTime)
	{
		switch (state_)
		{
		case State::GET_CHARACTER:
			WaitForGetCharacterResp();
			break;
		case State::SELECT_CHARACTER:
			SelectCharacterState();
			break;
		case State::WAIT_FOR_SELECT_CHARACTER_RESP:
			WaitForSelectCharacterResp();
			break;
		}

		return 0;
	}
	void SelectCharacterScene::SendSelectCharacterReq()
	{
		if (!currentSelectCharacterId_)
		{
			Log("SelectCharacterScene::SendSelectCharacterReq : Character not selected");
			return;
		}

		Network::SelectCharacterMsgReq characterSelectReq;
		characterSelectReq.id = charactersData_[currentSelectCharacterId_.constValue()].characterInfo.id_;
		gateway_.AddToOutbox(0, Network::CreateIMessagePtr<Network::SelectCharacterMsgReq>(characterSelectReq));
		selectedCharacterMsgSentTime_ = std::chrono::high_resolution_clock::now();
		Log("SelectCharacterScene::SendSelectCharacterReq : Character selected");
	}
	void SelectCharacterScene::WaitForSelectCharacterResp()
	{
		auto now = std::chrono::high_resolution_clock::now();
		if (std::chrono::duration_cast<std::chrono::seconds>(now - selectedCharacterMsgSentTime_).count() > 3 )
		{
			Log("SelectCharacterScene::WaitForSelectCharacterResp : time out.");
			state_ = State::SELECT_CHARACTER;
			return;
		}

		auto msg = gateway_.PopInBox();
		if (msg == nullptr)
			return;
		auto resp = Network::castMessageAs<Network::SelectCharacterMsgResp>(msg->second);

		if (resp->status_ == Network::MessageStatus::Fail)
		{
			Log(" SelectCharacterScene::WaitForSelectCharacterResp : server refused select that character " + std::to_string(resp->id));
			state_ = State::SELECT_CHARACTER;
			return;
		}

		std::string sceneName = "MainScene";

		switch(resp->mapId)
		{
		case 1: sceneName = "MainScene";
			break;
		}

		// id = game, set selected characeter
		gameContext_.selectedCharacterId = resp->id;
		GameEngine::SceneEvent e(GameEngine::SceneEventType::LOAD_SCENE_BY_NAME, sceneName);
		addSceneEvent(e);

		state_ = State::END;
	}
	void SelectCharacterScene::SendGetCharacter()
	{
		Network::GetCharactersMsgReq getCharactersMsgReq;
		gateway_.AddToOutbox(0, Network::CreateIMessagePtr<Network::GetCharactersMsgReq>(getCharactersMsgReq));
	}
	void SelectCharacterScene::WaitForGetCharacterResp()
	{
		Log("Wait for get my characters.");

		auto myCharacters = gateway_.PopInBox();

		if (myCharacters == nullptr)
			return;

		auto msg = Network::castMessageAs<Network::GetCharactersMsgResp>(myCharacters->second);

		if (msg == nullptr)
		{
			Log("SelectCharacterScene::WaitForGetCharacterResp, got msg but wrong type : " + std::to_string(msg->GetType()));
			return;
		}

		for (const auto& cd : msg->characterInfo)
		{
			if (!cd) continue;
			AddSlot(cd.constValue());
		}

		state_ = State::SELECT_CHARACTER;
	}
	void SelectCharacterScene::SlotToRenderer(const CharacterSlot& slot, uint32 id)
	{
		int i = 0;
		for (const auto& text : slot.texts)
		{
			auto name = "Slot_" + std::to_string(id) + "_" + std::to_string(i++);
			renderersManager_->GuiText(name) = text.second;
		}
	}
	void SelectCharacterScene::AddSlot(const Network::CharacterInfo& info)
	{
		vec2 offsetX(0.25f, 0.f);
		vec2 offsetY(0.0f, -0.1f);
		vec2 startPosition(-0.441818, -0.477143);
		float size = .5f;

		uint32 slotId = charactersData_.size();
		vec2 finalPos = startPosition + offsetX * static_cast<float>(slotId);

		CharacterSlot newSlot;
		newSlot.texts[CharacterSlot::TextType::NICK].text = info.GetName();
		newSlot.texts[CharacterSlot::TextType::NICK].m_size = size;
		newSlot.texts[CharacterSlot::TextType::NICK].colour = itemsTextColour_;
		newSlot.texts[CharacterSlot::TextType::NICK].position = finalPos + offsetY;
		newSlot.texts[CharacterSlot::TextType::LVL].m_size = size;
		newSlot.texts[CharacterSlot::TextType::LVL].text = "Lvl : " + std::to_string(info.lvl_);
		newSlot.texts[CharacterSlot::TextType::LVL].colour = itemsTextColour_;
		newSlot.texts[CharacterSlot::TextType::LVL].position = finalPos + 2.f*offsetY;
		newSlot.texts[CharacterSlot::TextType::CLASSNAME].m_size = size;
		newSlot.texts[CharacterSlot::TextType::CLASSNAME].text = "Class id : " + std::to_string(info.classId_);
		newSlot.texts[CharacterSlot::TextType::CLASSNAME].colour = itemsTextColour_;
		newSlot.texts[CharacterSlot::TextType::CLASSNAME].position = finalPos + 3.f*offsetY;;
		newSlot.characterInfo = info;
		SlotToRenderer(newSlot, slotId);
		charactersData_.push_back(newSlot);
	}
	void SelectCharacterScene::SetCurrentChoiceText()
	{
		if (charactersData_.empty() || !currentSelectCharacterId_)
			return;

		renderersManager_->GuiText("CurrentChoice") = characterSelectText_;
		renderersManager_->GuiText("CurrentChoice").position = vec2(-0.2, 0);
		renderersManager_->GuiText("CurrentChoice").text = charactersData_[currentSelectCharacterId_.value()].characterInfo.GetName();
	}
	void SelectCharacterScene::Pause()
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
	}
	void SelectCharacterScene::CheckRangeSelectedCharacter()
	{
		if (!currentSelectCharacterId_)
			return;
		
		auto& v = currentSelectCharacterId_.value();
		auto size = static_cast<uint8>(charactersData_.size());

		if (v >= size)
			v = size - 1;

		if (v < 0)
			v = 0;
	}
	void SelectCharacterScene::ChangeSelectedCharacter(int8 dir)
	{
		if (!currentSelectCharacterId_)
		{
			currentSelectCharacterId_ = 0;
			return;
		}

		currentSelectCharacterId_.value() += dir;
	}
	void SelectCharacterScene::SelectCharacterState()
	{
		if (inputManager_->GetKeyDown(KeyCodes::ENTER))
		{
			SendSelectCharacterReq();
			Pause();
			state_ = State::WAIT_FOR_SELECT_CHARACTER_RESP;
		}

		if (inputManager_->GetKeyDown(KeyCodes::A))
		{
			ChangeSelectedCharacter(-1);
			Pause();
		}

		if (inputManager_->GetKeyDown(KeyCodes::D))
		{
			ChangeSelectedCharacter(1);
			Pause();
		}

		CheckRangeSelectedCharacter();
		SetCurrentChoiceText();
	}
} // MmmoRpg
