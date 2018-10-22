#include "SelectCharacterScene.h"
#include "UtilsNetwork/Gateway.h"
#include "Renderers/GUI/Texutre/GuiTextureElement.h"
#include "Renderers/GUI/Text/GuiTextElement.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "States/GetCharacterState.h"
#include "States/SelectingCharacterState.h"
#include "States/WaitForConfirmationState.h"
#include "GameEngine/Resources/TextureLoader.h"
#include "GLM/GLMUtils.h"

namespace MmmoRpg
{
	SelectCharacterScene::SelectCharacterScene(Network::CGateway& gateway, const std::string& serverAddress, MrpgGameContext& gameContext)
		: MRpgScene("SelectCharacterScene", gateway, serverAddress, gameContext)
		, currentState_(0)
	{
	
	}
	SelectCharacterScene::~SelectCharacterScene()
	{
	}
	int SelectCharacterScene::Initialize()
	{
		GameEngine::Renderer::Gui::GuiTextureElement guiTexture;
		guiTexture.texture = resourceManager_->GetTextureLaoder().LoadTexture("GUI/character_select.jpg", false);
		renderersManager_->GuiTexture("bg") = guiTexture;

		renderersManager_->GuiText("SelectCharacter").text = "SelectCharacter";
		renderersManager_->GuiText("SelectCharacter").colour = Utils::RGBtoFloat(20.f, 50.f, 150.f);

		CreateState();
		return 0;
	}
	void SelectCharacterScene::PostInitialize()
	{

	}
	int SelectCharacterScene::Update(float deltaTime)
	{
		gateway_.Update();

		if (state_ == nullptr)
			return 0;

		state_->Update();

		switch (state_->GetStatus())
		{
		case StateStatus::DONE:
			++currentState_;
			CreateState();
			break;
		case StateStatus::BACK_TO_PREVIOUS_STATE:
			--currentState_;
			CreateState();
			break;
		case StateStatus::ON_GOING: break;
		}

		return 0;
	}
	void SelectCharacterScene::CreateState()
	{
		switch (currentState_)
		{
		case 0: state_ = std::make_unique<GetCharacterState>(dispatcher_, *renderersManager_, gateway_, charactersData_); return;
		case 1:	state_ = std::make_unique<SelectingCharacterState>(*inputManager_, *renderersManager_, gameContext_, charactersData_); return;
		case 2:	state_ = std::make_unique<WaitForConfirmationState>(dispatcher_, gateway_, gameContext_, addSceneEvent, charactersData_); return;
		default: state_ = nullptr;
		}
	}
} // MmmoRpg
