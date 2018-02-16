#include "SceneManager.h"
#include "Scene.hpp"
#include "../Renderers/GUI/GuiContext.h"
#include "../Renderers/RenderersManager.h"
#include "Logger/Log.h"

namespace GameEngine
{
	SceneManager::SceneManager(SceneFactoryBasePtr sceneFactory, std::shared_ptr<CDisplayManager>& displayManager, std::shared_ptr<InputManager>& inputManager, Renderer::RenderersManager& renderersManager, Renderer::Gui::GuiContext& guiContext)
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
	Scene* SceneManager::GetActiveScene()
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
	void SceneManager::SetActiveScene(const std::string& name)
	{
		LoadScene(name);
	}

	void SceneManager::Reset()
	{
		sceneWrapper_.Reset();
	}

	void SceneManager::UpadteScene(float dt)
	{
		if (!sceneWrapper_.IsInitialized())
			return;

		sceneWrapper_.Get()->FullUpdate(dt);
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
		if (currentSceneId_ >= sceneFactory_->ScenesSize() - 1)
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

	template<class T>
	void SceneManager::JustLoadScene(T scene)
	{
		renderersManager_.UnSubscribeAll();
		sceneWrapper_.Reset();
		auto s = sceneFactory_->Create(scene);
		SetSceneContext(s.get());
		sceneWrapper_.Set(std::move(s));
	}

	void SceneManager::LoadScene(uint32 id)
	{
		if (!sceneFactory_->IsExist(id))
		{
			Log("SceneManager::LoadScene() no more scenes found.");
			return;
		}
		currentSceneId_ = id;
		JustLoadScene<uint32>(id);
	}
	
	void SceneManager::LoadScene(const std::string & name)
	{
		if (!sceneFactory_->IsExist(name))
		{
			Log("SceneManager::LoadScene() " + name + " not found.");
			return;
		}

		currentSceneId_ = sceneFactory_->GetSceneId(name);
		JustLoadScene<const std::string&>(name);
	}
	void SceneManager::SetSceneContext(Scene* scene)
	{
		scene->SetAddSceneEventCallback(std::bind(&SceneManager::AddSceneEvent, this, std::placeholders::_1));
		scene->SetInputManager(inputManager_.get());
		scene->SetRenderersManager(&renderersManager_);		
		scene->SetDisplayManager(displayManager_.get());
	}

} // GameEngine