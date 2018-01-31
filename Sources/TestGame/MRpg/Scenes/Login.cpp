#include "Login.h"
#include "../UtilsNetwork/Gateway.h"
#include "../GameEngine/Engine/Engine.h"
#include "../GameEngine/Renderers/GUI/Text/GuiText.h"

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

		guiLogin_.text = "Login :";
		guiLogin_.colour = glm::vec3(0, 162.f / 255.f, 232.f / 255.f);
		guiLogin_.position = glm::vec2(-0.5, 0.2);

		guiPass_.text = "Password :";
		guiPass_.colour = glm::vec3(0, 162.f / 255.f, 232.f / 255.f);
		guiPass_.position = glm::vec2(-0.5, 0.0);
	}
	LoginScene::~LoginScene()
	{
	}
	int LoginScene::Initialize()
	{	
		login_ = "baszek";
		password_ = "haslo";
		renderersManager_->GuiText("login") = guiLogin_;
		renderersManager_->GuiText("pass") = guiPass_;
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
			renderersManager_->GuiText("login").text = "Login : " + login_ + (loginOrPasswordInput ? "" : "_");
	//	if (guiTexts.count("pass") > 0)
			renderersManager_->GuiText("pass").text = "Password : " + passwordToShow_ + (!loginOrPasswordInput ? "" : "_");;

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

			//gateway_.ConnectToServer(login_, password_, serverAddress_, 1991);
			tryToLogin = true;
			//engine_.AddEngineEvent(GameEngine::EngineEvent::LOAD_NEXT_SCENE);

			GameEngine::SceneEvent e(GameEngine::SceneEventType::LOAD_NEXT_SCENE);
			addSceneEvent(e);
		}

		for (const auto& p : inputManager_->GetCharKey())
			(*tmp) += p;

		passwordToShow_.clear();
		for (auto c : password_)
			passwordToShow_ += '*';

		return 0;
	}
}
