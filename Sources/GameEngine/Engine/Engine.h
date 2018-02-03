#pragma once
#include "../Scene/SceneManager.h"
#include "../Input/InputManager.h"
#include "../Display/DisplayManager.hpp"
#include "../Renderers/RenderersManager.h"
#include "IntroRenderer.h"
#include "ThreadSync.h"
#include "EngineEvent.h"

class CLoadingScreenRenderer;

namespace GameEngine
{
	class CDisplayManager;

	class CEngine
	{
	private:
		std::shared_ptr<CDisplayManager> displayManager;

	public:
		CEngine(SceneFactoryBasePtr sceneFactory);
		~CEngine();
		void Init();
		void GameLoop();
		void AddEngineEvent(EngineEvent event);
		CDisplayManager & GetDisplayManager();

	public:
		Renderer::Gui::GuiContext guiContext_;
		InputManagerPtr inputManager_;
		Renderer::RenderersManager renderersManager_;
		SceneManager sceneManager_;

	private:
		void ReadConfigFile(const std::string& file_name);
		void SetDisplay();
		ApiMessages::Type MainLoop();
		void ProcessEngineEvents();

		ApiMessages::Type PrepareFrame();

	private:
		IntroRenderer introRenderer_;
		std::mutex engineEventsMutex;
		std::list<EngineEvent> engineEvents;		
	};
} // GameEngine