#include "Login.h"
#include "../UtilsNetwork/Gateway.h"
#include "../GameEngine/Engine/Engine.h"

std::unordered_map<KeyCodes::Type, char> keys_;

namespace MmmoRpg
{
	LoginScene::LoginScene(GameEngine::CEngine & engine, Network::CGateway & gateway, const std::string& serverAddress)
		: engine_(engine)
		, gateway_(gateway)
		, serverAddress_(serverAddress)
	{
		loginOrPasswordInput = false;
		tryToLogin = false;

		CGUIRenderer* gui_renderer = new CGUIRenderer();
		guitexts = new CGuiText("GUI/consola.ttf", engine_.projection.GetWindowSize());
		gui_renderer->AddElement(guitexts);

		
		guiLogin_.text = "Login :";
		guiLogin_.colour = glm::vec3(0, 162.f / 255.f, 232.f / 255.f);
		guiLogin_.position = glm::vec2(-0.5, 0.2);

		guiPass_.text = "Password :";
		guiPass_.colour = glm::vec3(0, 162.f / 255.f, 232.f / 255.f);
		guiPass_.position = glm::vec2(-0.5, 0.0);

		guitexts->texts["login"] = guiLogin_;
		guitexts->texts["pass"] = guiPass_;

		engine_.renderers.emplace_back(gui_renderer);


		//keys_ =
		//{
		//	{ KeyCodes::Q, 'q'},
		//	{ KeyCodes::W, 'w' },
		//	{ KeyCodes::E, 'e' },
		//	{ KeyCodes::R, 'r' },
		//	{ KeyCodes::T, 't' },
		//	{ KeyCodes::Y, 'y' },
		//	{ KeyCodes::U, 'u' },
		//	{ KeyCodes::I, 'i' },
		//	{ KeyCodes::O, 'o' },
		//	{ KeyCodes::O, 'p' },
		//	{ KeyCodes::A, 'a' },
		//	{ KeyCodes::S, 's' },
		//	{ KeyCodes::Q, 'q' },
		//	{ KeyCodes::D, 'd' },
		//	{ KeyCodes::F, 'f' },
		//	{ KeyCodes::G, 'g' },
		//	{ KeyCodes::H, 'h' },
		//	{ KeyCodes::J, 'j' },
		//	{ KeyCodes::K, 'k' },
		//	{ KeyCodes::L, 'l' },
		//	{ KeyCodes::Z, 'z' },
		//	{ KeyCodes::X, 'x' },
		//	{ KeyCodes::C, 'c' },
		//	{ KeyCodes::V, 'v' },
		//	{ KeyCodes::B, 'b' },
		//	{ KeyCodes::N, 'n' },
		//	{ KeyCodes::M, 'm' },
		//};


	}
	LoginScene::~LoginScene()
	{
	}
	int LoginScene::Initialize()
	{	
		//return 0;
		//gateway_.ConnectToServer("baszek", "haslo", serverAddress_, 1991);
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
		if(guitexts->texts.count("login") > 0)
			guitexts->texts["login"].text = "Login : " + login_ + (loginOrPasswordInput ? "" : "_");
		if (guitexts->texts.count("pass") > 0)
			guitexts->texts["pass"].text = "Password : " + passwordToShow_ + (!loginOrPasswordInput ? "" : "_");;

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
			guitexts->texts.clear();
			engine_.renderers.pop_back();
		}

		for (const auto& p : engine_.inputManager.GetCharKey())
			(*tmp) += p;

		passwordToShow_.clear();
		for (auto c : password_)
			passwordToShow_ += '*';


		


		/*std::cout << "Login to server : " << std::endl;
		std::string username;
		std::string pass;

		std::cout << "User: ";
		std::cin >> username;
		std::cout << "Pass: ";
		std::cin >> pass;*/

		return 0;
	}
}
