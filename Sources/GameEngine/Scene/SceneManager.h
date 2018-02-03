#pragma once
#include "SceneFactoryBase.h"
#include "ThreadSync.h"
#include "SceneWrapper.h"
#include "SceneEvents.h"
#include "Mutex.hpp"
#include "optional.hpp"
#include <queue>

namespace GameEngine
{
	namespace Renderer
	{
		namespace Gui
		{
			struct GuiContext;
		}

		class RenderersManager;
	} // Renderer
	
	class InputManager;

	class SceneManager
	{
	public:
		SceneManager(SceneFactoryBasePtr, std::shared_ptr<CDisplayManager>&, std::shared_ptr<InputManager>&, Renderer::RenderersManager&, Renderer::Gui::GuiContext& guiContext);
		~SceneManager();
		Scene* GetActiveScene();
		void InitActiveScene();
		void RuntimeLoadObjectToGpu();
		void Update();		
		void SetActiveScene(const std::string& name);		


	private:
		void ProccessEvents();
		void UpadteScene(float dt);
		void AddSceneEvent(const SceneEvent&);
		wb::optional<GameEngine::SceneEvent> GetSceneEvent();

		void LoadNextScene();
		void LoadPreviousScene();
		void LoadScene(const std::string&);
		void LoadScene(uint32 id);
		void SetSceneContext(Scene* scene);
		
		template<class T>
		void JustLoadScene(T scene);

	private:
		SceneFactoryBasePtr sceneFactory_;

		uint32 currentSceneId_;
		SceneWrapper sceneWrapper_;

		std::mutex eventMutex_;
		std::queue<GameEngine::SceneEvent> events_;

		std::shared_ptr<CDisplayManager>& displayManager_;
		std::shared_ptr<InputManager>& inputManager_;
		Renderer::RenderersManager& renderersManager_;
		Renderer::Gui::GuiContext& guiContext_;

		Utils::Thread::ThreadSync threadSync_;
	};
} // GameEngine