#pragma once
#include "ISceneFactory.h"
#include "ThreadSync.h"
#include "SceneWrapper.h"
#include "SceneEvents.h"
#include "Mutex.hpp"
#include "optional.hpp"
#include <queue>

class CInputManager;

namespace GameEngine
{
	namespace GUI
	{
		struct GuiContext;
	}

	namespace Renderer
	{
		class RenderersManager;
	} // Renderer

	typedef std::vector<std::string> ScenesVec;

	class SceneManager
	{
	public:
		SceneManager(ISceneFactoryPtr, std::shared_ptr<CDisplayManager>&, CInputManager&, Renderer::RenderersManager&, GUI::GuiContext& guiContext);
		~SceneManager();
		CScene* GetActiveScene();
		void InitActiveScene();
		void RuntimeLoadObjectToGpu();
		void Update();		
		void AddScene(const std::string&);
		void SetActiveScene(const std::string& name);		
		wb::optional<uint32> GetSceneId(const std::string& name) const;

	private:
		void ProccessEvents();
		void UpadteScene(float dt);
		void AddSceneEvent(const SceneEvent&);
		wb::optional<GameEngine::SceneEvent> GetSceneEvent();

		void LoadNextScene();
		void LoadPreviousScene();
		void LoadScene(const std::string&);
		void LoadScene(uint32 id);

	private:
		ISceneFactoryPtr sceneFactory_;

		uint32 currentSceneId_;
		SceneWrapper sceneWrapper_;
		ScenesVec  scenes_;

		std::mutex eventMutex_;
		std::queue<GameEngine::SceneEvent> events_;

		std::shared_ptr<CDisplayManager>& displayManager_;
		CInputManager& inputManager_;
		Renderer::RenderersManager& renderersManager_;
		GUI::GuiContext& guiContext_;

		Utils::Thread::ThreadSync threadSync_;
	};
} // GameEngine