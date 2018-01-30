#pragma once
#include <memory>
#include <vector>
#include "Mutex.hpp"

class CScene;
class CDisplayManager;

typedef std::unique_ptr<CScene> ScenePtr;

namespace GameEngine
{
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

		CScene* Get();
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