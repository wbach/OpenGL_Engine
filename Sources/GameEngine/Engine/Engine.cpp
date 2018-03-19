#include "Engine.h"
#include "Configuration.h"
#include "Logger/Log.h"

namespace GameEngine
{
	CEngine::CEngine(IGraphicsApiPtr graphicsApi, SceneFactoryBasePtr sceneFactory)
		: displayManager(nullptr)
		, inputManager_(nullptr)
		, renderersManager_(graphicsApi)
		, sceneManager_(graphicsApi, sceneFactory, displayManager, inputManager_, renderersManager_, guiContext_)
		, introRenderer_(graphicsApi, displayManager)
		, graphicsApi_(graphicsApi)
		, isRunning(true)
	{
		graphicsApi_->SetBackgroundColor(vec3(.8f));
		ReadConfigFile("./Conf.xml");
		SetDisplay();
		sceneManager_.SetFactor();
	}

	CEngine::~CEngine()
	{
		sceneManager_.Reset();
		Log("");
		EngineConf_SaveRequiredFiles();
	}

	void CEngine::ReadConfigFile(const std::string & file_name)
	{
		EngineConf.ReadFromFile(file_name);
		EngineConf_AddRequiredFile(file_name);
	}

	void CEngine::SetDisplay()
	{
		auto& conf = EngineConf;
		
		WindowType type = WindowType::WINDOW;
		if (conf.fullScreen)
			type = WindowType::FULL_SCREEN;

		displayManager = std::make_shared<CDisplayManager>(graphicsApi_, conf.windowName, conf.resolution.x, conf.resolution.y, type);
		inputManager_ = displayManager->CreateInput();
		introRenderer_.Render();
	}

	void CEngine::GameLoop()
	{
		while (isRunning.load())
			MainLoop();
	}

	void CEngine::AddEngineEvent(EngineEvent event)
	{
		std::lock_guard<std::mutex> lk(engineEventsMutex);
		engineEvents.push_back(event);
	}

	void CEngine::Render()
	{
		renderersManager_.RenderScene(sceneManager_.GetActiveScene());
	}

	CDisplayManager &CEngine::GetDisplayManager()
	{
		return *displayManager;
	}

	void CEngine::MainLoop()
	{
		inputManager_->GetPressedKeys();
		sceneManager_.RuntimeLoadObjectToGpu();
		PrepareFrame();

		if (inputManager_->GetKey(KeyCodes::ESCAPE))
			isRunning.store(false);

		Render();
		sceneManager_.Update();

		ProcessEngineEvents();

		displayManager->Update();
	}

	void CEngine::ProcessEngineEvents()
	{
		EngineEvent event;
		{
			std::lock_guard<std::mutex> lk(engineEventsMutex);

			if (engineEvents.empty())
				return;

			event = engineEvents.front();
			engineEvents.pop_front(); 
		}

		switch (event)
		{
		case EngineEvent::LOAD_NEXT_SCENE:

			break;
		case EngineEvent::QUIT:
			break;
		}
	}

	void CEngine::PrepareFrame()
	{
		graphicsApi_->PrepareFrame();
		displayManager->ProcessEvents();
	}

	void CEngine::Init()
	{
		graphicsApi_->EnableDepthTest();
		renderersManager_.Init();
	}	
} // GameEngine
