#include "SceneWrapper.h"
#include "SceneLoader.h"
#include "Scene.hpp"
#include "Logger/Log.h"

namespace GameEngine
{
	SceneWrapper::SceneWrapper(std::shared_ptr<CDisplayManager>& displayManager)
		: displayManager_(displayManager)
		, state_(SceneWrapperState::SceneNotSet)
	{
	}

	void SceneWrapper::Set(ScenePtr scene)
	{
		SaveSetState(SceneWrapperState::SceneNotSet);

		activeScene.reset();
		activeScene = std::move(scene);

		SaveSetState(SceneWrapperState::ReadyToInitialized);
	}

	void SceneWrapper::Init()
	{
		std::lock_guard<std::mutex> lk(initMutex_);

		if (SaveGetState() != SceneWrapperState::ReadyToInitialized)
		{
			Error("SceneWrapper::Init() Wrong state.");
			return;
		}
		SaveSetState(SceneWrapperState::Initializing);

		SceneLoader sceneloader(displayManager_);
		if (!sceneloader.Load(activeScene.get()))
			return;

		SaveSetState(SceneWrapperState::Initilaized);
	}

	bool SceneWrapper::IsInitialized()
	{
		return SaveGetState() == SceneWrapperState::Initilaized;
	}

	SceneWrapperState SceneWrapper::SaveGetState()
	{
		std::lock_guard<std::mutex> lk(stateMutex_);
		return state_;
	}

	void SceneWrapper::SaveSetState(SceneWrapperState state)
	{
		std::lock_guard<std::mutex> lk(stateMutex_);
		state_ = state;
	}

	CScene* SceneWrapper::Get()
	{
		if (SaveGetState() == SceneWrapperState::Initilaized)
			return activeScene.get();

		if (SaveGetState() == SceneWrapperState::SceneNotSet)
			Log("SceneWrapper::Get() scene is nullptr. Probably are not set active scene.");

		return nullptr;
	}

	SceneWrapperState SceneWrapper::GetState()
	{
		if (activeScene == nullptr)
			return SceneWrapperState::SceneNotSet;

		return SaveGetState();
	}
} // GameEngine