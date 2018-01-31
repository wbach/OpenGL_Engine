#include "Login.h"
#include "../UtilsNetwork/Gateway.h"
#include "../GameEngine/Engine/Engine.h"
#include "../GameEngine/Renderers/GUI/Text/GuiText.h"
#include "../GameEngine/Renderers/GUI/Texutre/GuiTextureElement.h"
#include "../UtilsNetwork/Messages/Conntection/AuthenticationMessage.h"

std::unordered_map<KeyCodes::Type, char> keys_;

namespace MmmoRpg
{
	LoginScene::LoginScene(Network::CGateway& gateway, const std::string& serverAddress)
		: CScene("LoginScene")
		, gateway_(gateway)
		, serverAddress_(serverAddress)
	{
		loginOrPasswordInput = false;
		tryToLogin = false;

		vec3 textColor = glm::vec3(20.f / 255.f, 20.f / 255.f, 20.f  / 255.f);

		guiLogin_.text = "Login :";
		guiLogin_.colour = textColor;
		guiLogin_.position = glm::vec2(-0.249091, 0.051429);

		guiPass_.text = "Password :";
		guiPass_.colour = textColor;
		guiPass_.position = glm::vec2(-0.249091, -0.262857);
	}
	LoginScene::~LoginScene()
	{
	}
	int LoginScene::Initialize()
	{	
		GameEngine::Renderer::Gui::GuiTextureElement guiTexture;
		guiTexture.texture = resourceManager.GetTextureLaoder().LoadTexture("GUI/LoginBg_1.jpg", false);

		GameEngine::Renderer::Gui::GuiTextureElement loginBox;
		loginBox.texture = resourceManager.GetTextureLaoder().LoadTexture("GUI/login_password.jpg", false);
		loginBox.SetScale(vec2(.3f, .3f));

		login_ = "baszek";
		password_ = "haslo";
		renderersManager_->GuiText("login") = guiLogin_;
		renderersManager_->GuiText("pass") = guiPass_;
		renderersManager_->GuiTexture("bg") = guiTexture;
		renderersManager_->GuiTexture("loginBox") = loginBox;

		renderersManager_->GuiText("Error") = guiLogin_;
		renderersManager_->GuiText("Error").position.y -= 0.5f;
		renderersManager_->GuiText("Error").text = "";
		return 0;
	}
	void LoginScene::PostInitialize()
	{
	}

	int LoginScene::Update(float deltaTime)
	{
		/*if (tryToLogin)
			return 0;*/

	//	auto& guiTexts = engine_.renderersManager_.guiContext_.texts->texts;

		//if(guiTexts.count("login") > 0)
			renderersManager_->GuiText("login").text = login_ + (loginOrPasswordInput ? "" : "_");
	//	if (guiTexts.count("pass") > 0)
			renderersManager_->GuiText("pass").text = passwordToShow_ + (!loginOrPasswordInput ? "" : "_");;

		if (inputManager_->GetKeyDown(KeyCodes::TAB))
			loginOrPasswordInput = !loginOrPasswordInput;

		std::string* tmp = &login_;

		if (loginOrPasswordInput)
			tmp = &password_;

		if (inputManager_->GetKeyDown(KeyCodes::BACKSPACE))
			if(!tmp->empty())
				tmp->pop_back();		
		
		if (inputManager_->GetKeyDown(KeyCodes::ENTER))
		{
			if (login_.empty() || password_.empty())
				return 0;

			auto isConnected = gateway_.ConnectToServer(login_, password_, serverAddress_, 1991);

			if (isConnected)
			{
				renderersManager_->GuiText("Error").text = "Connected.";
				GameEngine::SceneEvent e(GameEngine::SceneEventType::LOAD_NEXT_SCENE);
				addSceneEvent(e);
			}
			else
			{
				renderersManager_->GuiText("Error").text = "Connection Error.";
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(200));
		}

		for (const auto& p : inputManager_->GetCharKey())
			(*tmp) += p;

		passwordToShow_.clear();
		for (auto c : password_)
			passwordToShow_ += '*';

		return 0;
	}
}
