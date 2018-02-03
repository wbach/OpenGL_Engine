#pragma once
#include "../GameEngine/Scene/Scene.hpp"
#include "../GameEngine/Renderers/GUI/GuiRenderer.h"
#include "../GameEngine/Renderers/GUI/Text/GuiTextElement.h"
#include "Time/TimeMeasurer.h"
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
	class LoginScene : public GameEngine::Scene
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
		void SwapLoginPassword();
		void SwapCursor();

	private:
		Network::CGateway& gateway_;
		std::string serverAddress_;

		Utils::Time::CTimeMeasurer cursorTimer_;

		SGuiTextElement guiLogin_;
		SGuiTextElement guiPass_;

		std::string login_;
		std::string password_;

		bool loginOrPasswordInput;
		bool showCursor_;
	};
} // MmmoRpg