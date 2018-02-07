#include "Engine.h"
#include "SingleTon.h"
#include "Configuration.h"
#include "../Api/ApiMessages.h"
#include <GL/glew.h>
#include "Logger/Log.h"
#include <fstream>

namespace GameEngine
{
	CEngine::CEngine(SceneFactoryBasePtr sceneFactory)
		: displayManager(nullptr)
		, sceneManager_(sceneFactory, displayManager, inputManager_, renderersManager_, guiContext_)
		, inputManager_(nullptr)
		, introRenderer_(displayManager)
	{
		ReadConfigFile("./Conf.xml");
		SetDisplay();		
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
		displayManager = std::make_shared<CDisplayManager>(conf.windowName, conf.resolution.x, conf.resolution.y, conf.fullScreen);
		inputManager_ = displayManager->GetApi()->CreateInput();
		introRenderer_.Render();
	}

	void CEngine::GameLoop()
	{
		ApiMessages::Type apiMessage = ApiMessages::NONE;

		while (apiMessage != ApiMessages::QUIT)
			apiMessage = MainLoop();
	}

	void CEngine::AddEngineEvent(EngineEvent event)
	{
		std::lock_guard<std::mutex> lk(engineEventsMutex);
		engineEvents.push_back(event);
	}

	CDisplayManager &CEngine::GetDisplayManager()
	{
		return *displayManager;
	}

	ApiMessages::Type CEngine::MainLoop()
	{
		ApiMessages::Type apiMessage = ApiMessages::NONE;

		inputManager_->GetPressedKeys();
		sceneManager_.RuntimeLoadObjectToGpu();
		apiMessage = PrepareFrame();

		if (inputManager_->GetKey(KeyCodes::ESCAPE))
			apiMessage = ApiMessages::QUIT;

		renderersManager_.RenderScene(sceneManager_.GetActiveScene());
		sceneManager_.Update();

		ProcessEngineEvents();

		displayManager->Update();

		return apiMessage;
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

	ApiMessages::Type CEngine::PrepareFrame()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(.8f, .8f, .8f, 1.f);
		return displayManager->PeekApiMessage();
	}

	void CEngine::Init()
	{
		glEnable(GL_DEPTH_TEST);
		renderersManager_.Init();	
	}	
} // GameEngine