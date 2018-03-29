#pragma once
#include "../Scene/SceneManager.h"
#include "../Input/InputManager.h"
#include "../Display/DisplayManager.hpp"
#include "../Renderers/RenderersManager.h"
#include "GameEngine/Api/IGraphicsApi.h"
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
		CEngine(IGraphicsApiPtr graphicsApi, SceneFactoryBasePtr sceneFactory);
		~CEngine();
		void Init();
		void GameLoop();
		void AddEngineEvent(EngineEvent event);
		void Render();
		CDisplayManager& GetDisplayManager();

	public:
		Renderer::Gui::GuiContext guiContext_;
		InputManagerPtr inputManager_;
		Renderer::RenderersManager renderersManager_;
		SceneManager sceneManager_;

	private:
		void SetDisplay();
		void MainLoop();
		void ProcessEngineEvents();
		void PrepareFrame();

	private:
		IGraphicsApiPtr graphicsApi_;
		IntroRenderer introRenderer_;
		std::mutex engineEventsMutex;
		std::list<EngineEvent> engineEvents;
		std::atomic_bool isRunning;
	};
} // GameEngine