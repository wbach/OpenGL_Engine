#pragma once
#include <memory>
#include <vector>

class CScene;
class CDisplayManager;

typedef std::shared_ptr<CScene> ScenePtr;
typedef std::vector<ScenePtr> ScenesVec;


namespace GameEngine
{
	class SceneWrapper
	{
	public:
		SceneWrapper(std::shared_ptr<CDisplayManager>&);

		CScene* Get();
		bool IsInitialized();
		void Set(ScenePtr scene);

	private:
		ScenePtr activeScene;
		std::shared_ptr<CDisplayManager>& displayManager_;
		bool isInitialized = false;
	};

} // GameEngine