#pragma once
#include "../Display/DisplayManager.hpp"
#include "../Input/InputManager.h"
#include "../Resources/ResourceManager.h"
#include "../Renderers/GUI/GuiContext.h"
#include "Projection.h"
#include "ThreadSync.h"
#include "EngineEvent.h"

class CScene;
class CRenderer;
class CDisplayManager;
class CLoadingScreenRenderer;

namespace GameEngine
{
	class CEngine
	{
	private:
		std::unique_ptr<CDisplayManager> displayManager;

	public:
		CEngine();
		~CEngine();
		void Init();
		void PreperaScene();
		void GameLoop();
		void LoadNextScene();
		void AddEngineEvent(EngineEvent event);

	public:
		GUI::GuiContext gui;
		CDisplayManager & GetDisplayManager();
		CProjection projection;
		CInputManager inputManager;
		std::shared_ptr<CScene> scene;
		std::vector<std::shared_ptr<CScene>> scenes;
		std::vector<std::shared_ptr<CRenderer>> renderers;

	private:
		void ReadConfigFile(const std::string& file_name);
		void SetDisplay();
		void InitGuiRenderer();
		ApiMessages::Type MainLoop();
		void SetDefaultRenderer();
		void LoadScene();
		void InitRenderers();
		void RenderScene();
		void Render(CRenderer*);
		void LoadObjects();
		void ProcessEngineEvents();

		ApiMessages::Type ProccesScene();
		ApiMessages::Type CheckSceneMessages(float deltaTime);
		ApiMessages::Type PrepareFrame();

	private:
		uint32 currentSceneId;
		Utils::Thread::ThreadSync threadSync;

		std::atomic_bool isLoading;
		std::mutex engineEventsMutex;
		std::list<EngineEvent> engineEvents;
		// Global items, loading screen etc...
		CResourceManager resorceManager;
	};
} // GameEngine