#pragma once
#include "../GameEngine/Scene/Scene.hpp"
#include "../GameEngine/Renderers/GUI/GuiRenderer.h"
#include "../GameEngine/Renderers/GUI/Text/GuiTextElement.h"
#include <unordered_map>

namespace GameEngine
{
	class CEngine;
} // GameEngine

namespace Network
{
	class CGateway;
} // Network

namespace MmmoRpg
{
	class LoginScene : public CScene
	{
	public:
		LoginScene(Network::CGateway& gateway, const std::string& serverAddress);
		virtual int		Initialize() override;
		virtual void	PostInitialize() override;
		virtual int		Update(float deltaTime) override;

	private:
		void PrintLoginAndPassword();
		void ConnectToServer();
		std::string PasswordToStars();
		void InsertText();
		void SwapLoginPassword();
		void Pause();

	private:
		Network::CGateway& gateway_;
		std::string serverAddress_;

		SGuiTextElement guiLogin_;
		SGuiTextElement guiPass_;

		std::string login_;
		std::string password_;

		bool loginOrPasswordInput;
		bool tryToLogin;
	};
} // MmmoRpg