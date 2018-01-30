#include "SceneManager.h"
#include "Scene.hpp"
#include "../Renderers/GUI/GuiContext.h"
#include "../Renderers/RenderersManager.h"
#include "Logger/Log.h"

namespace GameEngine
{
	SceneManager::SceneManager(ISceneFactoryPtr sceneFactory, std::shared_ptr<CDisplayManager>& displayManager, CInputManager& inputManager, Renderer::RenderersManager& renderersManager, GUI::GuiContext& guiContext)
		: sceneFactory_(sceneFactory)
		, displayManager_(displayManager)
		, inputManager_(inputManager)
		, renderersManager_(renderersManager)
		, guiContext_(guiContext)
		, sceneWrapper_(displayManager)
		, currentSceneId_(0)
	{
		threadSync_.Subscribe(std::bind(&SceneManager::UpadteScene, this, std::placeholders::_1));
		threadSync_.Start();
	}
	SceneManager::~SceneManager()
	{
		threadSync_.Stop();
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
	void SceneManager::Update()
	{
		if (sceneWrapper_.GetState() == SceneWrapperState::ReadyToInitialized)
			sceneWrapper_.Init();

		ProccessEvents();
	}
	void SceneManager::ProccessEvents()
	{
		auto optionalEvent = GetSceneEvent();

		if (!optionalEvent)
			return;

		auto e = optionalEvent.value();

		switch (e.type)
		{
		case SceneEventType::LOAD_NEXT_SCENE:		LoadNextScene();	 break;
		case SceneEventType::LOAD_PREVIOUS_SCENE:	LoadPreviousScene(); break;
		case SceneEventType::LOAD_SCENE_BY_ID:		LoadScene(e.id);	 break;
		case SceneEventType::LOAD_SCENE_BY_NAME:	LoadScene(e.name);	 break;
		}

	}
	void SceneManager::AddScene(const std::string& scene)
	{
		scenes_.emplace_back(scene);
	}
	void SceneManager::SetActiveScene(const std::string & name)
	{
		auto id = GetSceneId(name);

		if (!id)
		{
			Log("SceneManager::SetActiveScene scene \"" + name + "\" not found.");
			return;
		}

		currentSceneId_ = id.value();
		LoadScene(currentSceneId_);
	}
	wb::optional<uint32> SceneManager::GetSceneId(const std::string & name) const
	{
		uint32 i = 0;
		for (const auto& scene : scenes_)
		{
			if (scene == name)
				return i;
			++i;
		}
		return wb::optional<uint32>();
	}
	void SceneManager::UpadteScene(float dt)
	{
		if (!sceneWrapper_.IsInitialized())
			return;

		sceneWrapper_.Get()->Update(dt);
	}
	void SceneManager::AddSceneEvent(const SceneEvent& e)
	{
		std::lock_guard<std::mutex> lk(eventMutex_);
		events_.push(e);
	}

	wb::optional<GameEngine::SceneEvent> SceneManager::GetSceneEvent()
	{
		std::lock_guard<std::mutex> lk(eventMutex_);

		if (events_.empty())
			return wb::optional<GameEngine::SceneEvent>();

		auto e = events_.front();
		events_.pop();

		return e;
	}
	void SceneManager::LoadNextScene()
	{
		if (currentSceneId_ >= scenes_.size() - 1)
		{
			Log("SceneManager::LoadNextScene() no more scenes found.");
			return;
		}
		LoadScene(++currentSceneId_);
	}
	void SceneManager::LoadPreviousScene()
	{
		if (currentSceneId_ == 0)
		{
			Log("SceneManager::LoadPreviousScene() no more scenes found.");
			return;
		}
		LoadScene(--currentSceneId_);
	}
	void SceneManager::LoadScene(const std::string & name)
	{
		auto sceneId = GetSceneId(name);
		if (!sceneId)
		{
			Log("SceneManager::LoadScene() " + name + " not found.");
			return;
		}
		LoadScene(sceneId.value());
	}
	void SceneManager::LoadScene(uint32 id)
	{
		if (currentSceneId_ >= scenes_.size())
		{
			Log("SceneManager::LoadScene() no more scenes found.");
			return;
		}
		renderersManager_.UnSubscribeAll();

		auto scene = sceneFactory_->Create(scenes_[id]);
		scene->SetAddSceneEventCallback(std::bind(&SceneManager::AddSceneEvent, this, std::placeholders::_1));
		scene->SetGuiContext(&guiContext_);
		scene->SetInputManager(&inputManager_);
		scene->SetRenderersManager(&renderersManager_);
		sceneWrapper_.Set(std::move(scene));
	}
} // GameEngine