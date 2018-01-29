#include "SceneManager.h"
#include "Scene.hpp"
#include "Logger/Log.h"

namespace GameEngine
{
	SceneManager::SceneManager(std::shared_ptr<CDisplayManager>& displayManager, CInputManager& inputManager, Renderer::RenderersManager& renderersManager)
		: inputManager_(inputManager)
		, displayManager_(displayManager)
		, renderersManager_(renderersManager)
		, sceneWrapper_(displayManager)
	{
	}
	CScene* SceneManager::GetActiveScene()
	{
		return sceneWrapper_.Get();
	}
	void SceneManager::InitActiveScene()
	{
		if (!sceneWrapper_.IsInitialized())
		{
			Log("SceneManager::InitActiveScene() active scene isn't initialized.");
			return;
		}
	}
	void SceneManager::RuntimeLoadObjectToGpu()
	{
		if (!sceneWrapper_.IsInitialized())
			return;

		auto obj = sceneWrapper_.Get()->GetResourceManager().GetOpenGlLoader().GetObjectToOpenGLLoadingPass();
		if (obj != nullptr && !obj->isInOpenGL())
			obj->OpenGLLoadingPass();
	}
	void SceneManager::AddScene(CScene* scene)
	{
		scenes_.emplace_back(scene);
	}
	void SceneManager::SetActiveScene(const std::string & name)
	{
		for (auto& scene : scenes_)
		{
			if (scene->GetName() == name)
			{
				sceneWrapper_.Set(scene);
				return;
			}
		}
		Log("SceneManager::SetActiveScene scene \"" + name + "\" not found." );
	}
} // GameEngine