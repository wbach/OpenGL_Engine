#pragma once
#include <memory>
#include <vector>
#include "Mutex.hpp"

namespace GameEngine
{
	class Scene;
	class CDisplayManager;
	typedef std::unique_ptr<Scene> ScenePtr;
	
	enum SceneWrapperState
	{
		SceneNotSet,
		ReadyToInitialized,
		Initializing,
		Initilaized
	};

	class SceneWrapper
	{
	public:
		SceneWrapper(std::shared_ptr<CDisplayManager>&);

		Scene* Get();
		SceneWrapperState GetState();
		void Set(ScenePtr scene);
		void Init();
		bool IsInitialized();

	private:
		SceneWrapperState SaveGetState();
		void SaveSetState(SceneWrapperState state);

	private:
		std::mutex initMutex_;
		std::mutex stateMutex_;

		ScenePtr activeScene;
		std::shared_ptr<CDisplayManager>& displayManager_;
		SceneWrapperState state_;
	};
} // GameEngine