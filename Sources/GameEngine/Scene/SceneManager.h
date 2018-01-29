#pragma once
#include "ThreadSync.h"
#include "SceneWrapper.h"
#include "Mutex.hpp"
#include "SceneEvents.h"

class CInputManager;

namespace GameEngine
{
	namespace Renderer
	{
		class RenderersManager;
	} // Renderer
	
	class SceneManager
	{
	public:
		SceneManager(std::shared_ptr<CDisplayManager>&, CInputManager&, Renderer::RenderersManager&);
		CScene* GetActiveScene();
		void InitActiveScene();
		void RuntimeLoadObjectToGpu();
		void AddScene(CScene*);
		void SetActiveScene(const std::string& name);
		void AddSceneEvent(const SceneEvent&);

	private:
		SceneWrapper sceneWrapper_;
		ScenesVec scenes_;
		std::mutex eventMutex_;


		CInputManager& inputManager_;
		std::shared_ptr<CDisplayManager>& displayManager_;
		Renderer::RenderersManager& renderersManager_;

		Utils::Thread::ThreadSync threadSync;
	};
} // GameEngine