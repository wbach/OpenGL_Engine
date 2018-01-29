#include "Login.h"
#include "../UtilsNetwork/Gateway.h"
#include "../GameEngine/Engine/Engine.h"

std::unordered_map<KeyCodes::Type, char> keys_;

namespace MmmoRpg
{
	LoginScene::LoginScene(GameEngine::CEngine & engine, Network::CGateway & gateway, const std::string& serverAddress)
		: CScene("LoginScene")
		, engine_(engine)
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

		engine_.renderersManager_.GuiText("login") = guiLogin_;
		engine_.renderersManager_.GuiText("pass") = guiPass_;

	}
	LoginScene::~LoginScene()
	{
	}
	int LoginScene::Initialize()
	{	
		login_ = "baszek";
		password_ = "haslo";
		return 0;
	}
	void LoginScene::PostInitialize()
	{
	}

	int LoginScene::Update(float deltaTime)
	{
		if (tryToLogin)
			return 0;

	//	auto& guiTexts = engine_.renderersManager_.guiContext_.texts->texts;

		//if(guiTexts.count("login") > 0)
			engine_.renderersManager_.GuiText("login").text = "Login : " + login_ + (loginOrPasswordInput ? "" : "_");
	//	if (guiTexts.count("pass") > 0)
			engine_.renderersManager_.GuiText("pass").text = "Password : " + passwordToShow_ + (!loginOrPasswordInput ? "" : "_");;

		if (engine_.inputManager.GetKeyDown(KeyCodes::TAB))
			loginOrPasswordInput = !loginOrPasswordInput;

		std::string* tmp = &login_;

		if (loginOrPasswordInput)
			tmp = &password_;

		if (engine_.inputManager.GetKeyDown(KeyCodes::BACKSPACE))
			if(!tmp->empty())
				tmp->pop_back();		
		
		if (engine_.inputManager.GetKeyDown(KeyCodes::ENTER) && !tryToLogin)
		{
			if (login_.empty() || password_.empty())
				return 0;

			gateway_.ConnectToServer(login_, password_, serverAddress_, 1991);
			tryToLogin = true;
			engine_.AddEngineEvent(GameEngine::EngineEvent::LOAD_NEXT_SCENE);
			//guiTexts.clear();
			//engine_.renderers.pop_back();
		}

		for (const auto& p : engine_.inputManager.GetCharKey())
			(*tmp) += p;

		passwordToShow_.clear();
		for (auto c : password_)
			passwordToShow_ += '*';

		return 0;
	}
}
