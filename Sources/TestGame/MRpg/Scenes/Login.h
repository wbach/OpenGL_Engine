#pragma once
#include "../GameEngine/Scene/Scene.hpp"
#include "../GameEngine/Renderers/GUI/GuiRenderer.h"
#include <unordered_map>

namespace GameEngine
{
	class CEngine;
} // GameEngine

namespace Network
{
	class CGateway;
}

namespace MmmoRpg
{
	class LoginScene : public CScene
	{
	public:
		LoginScene(GameEngine::CEngine& engine, Network::CGateway& gateway, const std::string& serverAddress);
		virtual ~LoginScene() override;
		virtual int		Initialize() override;
		virtual void	PostInitialize() override;
		virtual int		Update(float deltaTime) override;

	private:
		GameEngine::CEngine& engine_;
		Network::CGateway& gateway_;
		std::string serverAddress_;

		CGuiText* guitexts;
		SGuiTextElement guiLogin_;
		SGuiTextElement guiPass_;

		std::string login_;
		std::string password_;
		std::string passwordToShow_;
		
		bool loginOrPasswordInput;
		bool tryToLogin;
	};
}