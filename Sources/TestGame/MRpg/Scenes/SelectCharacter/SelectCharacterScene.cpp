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
	SelectCharacterScene::SelectCharacterScene(Network::CGateway& gateway)
		: CScene("SelectCharacterScene")
		, gateway_(gateway)
	{
		characterSelectText_.text = "Select Character";
		characterSelectText_.colour = glm::vec3(0, 162.f / 255.f, 232.f / 255.f);
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

		SGuiTextElement characterSelectText_;

		auto textColour = glm::vec3(0, 162.f / 255.f, 232.f / 255.f);

		//characterSelectText_.text = "Slot 1";
		//characterSelectText_.colour = textColour;
		//characterSelectText_.position = vec2(-0.925455, -0.477143);
		//renderersManager_->GuiText("slot1") = characterSelectText_;

		characterSelectText_.text = "Slot 2";
		characterSelectText_.colour = textColour;
		characterSelectText_.position = vec2(-0.441818, -0.477143);
		renderersManager_->GuiText("slot2") = characterSelectText_;

		//characterSelectText_.text = "Slot 3";
		//characterSelectText_.colour = textColour;
		//characterSelectText_.position = vec2(0.0327272, -0.474286);
		//renderersManager_->GuiText("slot3") = characterSelectText_;

		//characterSelectText_.text = "Slot 4";
		//characterSelectText_.colour = textColour;
		//characterSelectText_.position = vec2(0.516364, -0.471429);
		//renderersManager_->GuiText("slot4") = characterSelectText_;

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


		return 0;

		Network::GetCharactersMsgReq getCharactersMsgReq;
		gateway_.AddToOutbox(0, Network::CreateIMessagePtr<Network::GetCharactersMsgReq>(getCharactersMsgReq));


		Log("Wait for get my characters.");
		std::this_thread::sleep_for(std::chrono::milliseconds(2000));

		auto myCharacters = gateway_.PopInBox();

		if (myCharacters)
		{
			auto msg = dynamic_cast<Network::GetCharactersMsgResp*>(myCharacters->second.get());
			if (msg)
			{
				Log("My characters : ");
				Log(msg->ToString());
				for (const auto& cd : msg->characterInfo)
				{
					if (!cd) continue;
					charactersData_.push_back(cd.value());
				}
			}
		}
		Log("SelectCharacterScene::Initialized");

		return 0;
	}
	void SelectCharacterScene::PostInitialize()
	{
		return;
		Network::SelectCharacterMsgReq characterSelectReq;
		characterSelectReq.id = 101;
		gateway_.AddToOutbox(0, Network::CreateIMessagePtr<Network::SelectCharacterMsgReq>(characterSelectReq));
		Log("Wait for character select...");
		std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	}
	int SelectCharacterScene::Update(float deltaTime)
	{
		if (!inputManager_)
		{
			Log("SelectCharacterScene::Update inputManager_ is null.");
			return 0;
		}
		if (inputManager_->GetKeyDown(KeyCodes::ENTER))
		{
			GameEngine::SceneEvent e(GameEngine::SceneEventType::LOAD_NEXT_SCENE, "MainScene");
			addSceneEvent(e);
		}
		return 0;
	}
}
