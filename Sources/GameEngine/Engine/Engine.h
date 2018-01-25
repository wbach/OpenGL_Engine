#pragma once
#include "../Display/DisplayManager.hpp"
#include "../Input/InputManager.h"
#include "../Resources/ResourceManager.h"
#include "Projection.h"
#include "ThreadSync.h"
#include "EngineEvent.h"

class CScene;
class CRenderer;
class CDisplayManager;
class CLoadingScreenRenderer;

namespace GameEngine
{
	enum class RenderersTypes
	{
		ObjectsRenderer,
		GuiRenderer
	};

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
		void AddRenderer(RenderersTypes type, CRenderer* renderer);

	public:
		CDisplayManager & GetDisplayManager();
		CProjection projection;
		CInputManager inputManager;
		std::shared_ptr<CScene> scene;
		std::vector<std::shared_ptr<CScene>> scenes;
		std::vector<std::unique_ptr<CRenderer>> renderers;
		std::unordered_map<RenderersTypes, uint32> rendererTypesMap;

	private:
		void ReadConfigFile(const std::string& file_name);
		void SetDisplay();
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