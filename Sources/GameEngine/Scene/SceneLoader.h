#pragma once
#include "GameEngine/Resources/ResourceManager.h"
#include "GameEngine/Api/IGraphicsApi.h"
#include "Thread.hpp"
#include "Mutex.hpp"
#include <memory>

class COpenGLObject;
class CResourceManager;

namespace GameEngine
{
	class Scene;
	class CDisplayManager;
	class LoadingScreenRenderer;

	class SceneLoader
	{
	public:
		SceneLoader(IGraphicsApiPtr graphicsApi, std::shared_ptr<CDisplayManager>& displayManager);
		~SceneLoader();
		bool Load(Scene* scene);

	private:
		void Init();
		void LoadScene(Scene* scene);
		void SetIsLoading(bool is);
		bool GetIsLoading();
		bool ProccesLoadingLoop(COpenGLObject * obj);
		bool LoadObject(COpenGLObject * obj);
		void UpdateScreen();
		void LoadingLoop(Scene* scene);
		void CheckObjectCount(Scene* scene);
		void PostLoadingPass(Scene* scene);
		void OpenGLLoadingPass(Scene* scene, std::thread& loading_thread);

	private:
		IGraphicsApiPtr graphicsApi_;
		int objectCount;
		int objectLoaded;
		std::unique_ptr<LoadingScreenRenderer> loadingScreenRenderer;

		bool isLoading;
		std::mutex loadingMutex;

		std::shared_ptr<CDisplayManager>& displayManager;
		ResourceManager resorceManager;
	};
} // GameEngine