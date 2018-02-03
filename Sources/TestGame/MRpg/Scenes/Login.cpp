#include "Login.h"
#include "../UtilsNetwork/Gateway.h"
#include "../GameEngine/Engine/Engine.h"
#include "../GameEngine/Renderers/GUI/Text/GuiText.h"
#include "../GameEngine/Renderers/GUI/Texutre/GuiTextureElement.h"
#include "../UtilsNetwork/Messages/Conntection/AuthenticationMessage.h"
#include "../GameEngine/Input/KeyCodeToCharConverter.h"

namespace MmmoRpg
{
	//TO DO: remove pause when fix getdown key in input manager
	LoginScene::LoginScene(Network::CGateway& gateway, const std::string& serverAddress)
		: GameEngine::Scene("LoginScene")
		, gateway_(gateway)
		, serverAddress_(serverAddress)
		, cursorTimer_(60, false, 250)
		, loginOrPasswordInput(false)
		, showCursor_(false)
	{
		cursorTimer_.AddOnTickCallback(std::bind(&LoginScene::SwapCursor, this));

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

		inputManager_->SubscribeOnKeyDown(KeyCodes::ENTER, [&]()
		{
			ConnectToServer();
		});

		inputManager_->SubscribeOnKeyDown(KeyCodes::TAB, [&]()
		{
			SwapLoginPassword();
		});

		inputManager_->SubscribeOnKeyDown(KeyCodes::BACKSPACE, [&]()
		{
			std::string* tmp = loginOrPasswordInput ? &password_ : &login_;

			if (!tmp->empty())
				tmp->pop_back();
		});

		inputManager_->SubscribeOnAnyKeyPress([&](KeyCodes::Type key)
		{
			std::string* tmp = loginOrPasswordInput ? &password_ : &login_;
			auto c = GameEngine::KeyCodeToCharConverter::Convert(key);

			if (c != 0)
				(*tmp) += c;
		});

		return 0;
	}
	void LoginScene::PostInitialize()
	{
	}

	int LoginScene::Update(float deltaTime)
	{
		PrintLoginAndPassword();
		cursorTimer_.CalculateAndLock();
		return 0;
	}

	void LoginScene::PrintLoginAndPassword()
	{
		renderersManager_->GuiText("login").text = login_ + ((!loginOrPasswordInput && showCursor_)? "_" : "");
		renderersManager_->GuiText("pass").text = PasswordToStars() + ((loginOrPasswordInput && showCursor_) ? "_" : "");
	}

	void LoginScene::ConnectToServer()
	{
		if (login_.empty() || password_.empty())
			return;

		auto isConnected = gateway_.ConnectToServer(login_, password_, serverAddress_, 1991);

		if (!isConnected)
		{
			renderersManager_->GuiText("Error").text = "Connection Error.";
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

	void LoginScene::SwapLoginPassword()
	{
		loginOrPasswordInput = !loginOrPasswordInput;
	}
	void LoginScene::SwapCursor()
	{
		showCursor_ = !showCursor_;
	}

} // MmmoRpg
