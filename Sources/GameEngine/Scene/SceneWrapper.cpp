#include "SceneWrapper.h"
#include "SceneLoader.h"
#include "Scene.hpp"
#include "Logger/Log.h"

namespace GameEngine
{
	SceneWrapper::SceneWrapper(std::shared_ptr<CDisplayManager>& displayManager)
		: displayManager_(displayManager)
	{
	}

	void SceneWrapper::Set(ScenePtr scene)
	{
		activeScene = scene;
	}

	CScene* SceneWrapper::Get()
	{
		if (isInitialized)
			return activeScene.get();

		if (!activeScene)
		{
			Log("SceneWrapper::Get() scene is nullptr. Probably are not set active scene.");
			return nullptr;
		}	

		SceneLoader sceneloader(displayManager_);
		if (!sceneloader.Load(activeScene.get()))
			return nullptr;

		isInitialized = true;
		return activeScene.get();
	}

	bool SceneWrapper::IsInitialized()
	{
		return isInitialized;
	}
} // GameEngine