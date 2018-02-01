#include "Login.h"
#include "../UtilsNetwork/Gateway.h"
#include "../GameEngine/Engine/Engine.h"
#include "../GameEngine/Renderers/GUI/Text/GuiText.h"
#include "../GameEngine/Renderers/GUI/Texutre/GuiTextureElement.h"
#include "../UtilsNetwork/Messages/Conntection/AuthenticationMessage.h"

namespace MmmoRpg
{
	//TO DO: remove pause when fix getdown key in input manager
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
		PrintLoginAndPassword();

		if (inputManager_->GetKeyDown(KeyCodes::ENTER))
			ConnectToServer();

		if (inputManager_->GetKeyDown(KeyCodes::TAB))
			SwapLoginPassword();

		InsertText(); // TO DO: move to game engine

		return 0;
	}

	void LoginScene::PrintLoginAndPassword()
	{
		renderersManager_->GuiText("login").text = login_ + (loginOrPasswordInput ? "" : "_");
		renderersManager_->GuiText("pass").text = PasswordToStars() + (!loginOrPasswordInput ? "" : "_");
	}
	void LoginScene::ConnectToServer()
	{
		if (login_.empty() || password_.empty())
			return;

		auto isConnected = gateway_.ConnectToServer(login_, password_, serverAddress_, 1991);

		if (!isConnected)
		{
			renderersManager_->GuiText("Error").text = "Connection Error.";
			Pause();
			return;
		}

		renderersManager_->GuiText("Error").text = "Connected.";
		GameEngine::SceneEvent e(GameEngine::SceneEventType::LOAD_NEXT_SCENE);
		addSceneEvent(e);
	}
	std::string LoginScene::PasswordToStars()
	{
		std::string passwordToShow;
		for (auto c : password_)
			passwordToShow += '*';
		return passwordToShow;
	}
	void LoginScene::InsertText()
	{
		std::string* tmp = &login_;

		if (loginOrPasswordInput)
			tmp = &password_;

		if (inputManager_->GetKeyDown(KeyCodes::BACKSPACE))
		{
			if(!tmp->empty())
				tmp->pop_back();
			Pause();
		}

		for (const auto& p : inputManager_->GetCharKey())
		{
			(*tmp) += p;
			Pause();
		}

	}
	void LoginScene::SwapLoginPassword()
	{
		loginOrPasswordInput = !loginOrPasswordInput;
		Pause();
	}
	void LoginScene::Pause()
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
	}
} // MmmoRpg
