#include "Engine.h"
#include "Configuration.h"
#include "../Api/ApiMessages.h"
#include "../Scene/Scene.hpp"
#include "../Renderers/FullRenderer.h"
#include "../Renderers/SimpleRenderer.h"
#include "../Renderers/GUI/GuiRenderer.h"
#include "../Debug_/Log.h"
#include <fstream>

CEngine::CEngine()
{
	auto& conf = SConfiguration::Instance();
	conf.ReadFromFile("../Data/Conf.xml");	

	displayManager = CDisplayManager(conf.windowName, conf.resolution.x, conf.resolution.y, conf.fullScreen);
	displayManager.SetInput(inputManager.input);
	projection = conf.resolution;
}

void CEngine::GameLoop()
{
	ApiMessages::Type m_ApiMessage = ApiMessages::NONE;

	while (m_ApiMessage != ApiMessages::QUIT)
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(.8f, .8f, .8f, 1.f);

		m_ApiMessage = displayManager.PeekMessage();

		if (inputManager.GetKey(KeyCodes::ESCAPE))
			m_ApiMessage = ApiMessages::QUIT;		

		if (scene != nullptr)
		{
			switch (scene->Update())
			{
			case 1: m_ApiMessage = ApiMessages::QUIT; break;
			}

			for (auto& renderer : renderers)
			{
				renderer->PrepareFrame(scene.get());
				renderer->Render(scene.get());
				renderer->EndFrame(scene.get());
			}						
		}
		displayManager.Update();
		inputManager.CheckReleasedKeys();
	}
}

void CEngine::OpenGLLoadingPass(std::thread& loading_thread)
{
    auto object_count = scene->objectCount;
	auto object_loaded = 0;

	if (object_count <= 0)
		object_count = 1;

	bool  load = true;
	displayManager.GetSync() = true;

	while (load)
	{
		displayManager.PeekMessage();

		load = GetIsLoading();

		auto obj = scene->GetResourceManager().GetOpenGlLoader().GetObjectToOpenGLLoadingPass();
		if (obj != nullptr)
		{
			load = true;
			obj->OpenGLLoadingPass();
			object_loaded++;

			std::cout << "Loading... " + std::to_string((int)((float)object_loaded / (float)object_count) *100.f) + "%" << std::endl;
		}

		loadingScreenRenderer->Render(nullptr);
		displayManager.Update();
	}
	loading_thread.join();
	load = true;
	while (load)
	{
		displayManager.PeekMessage();
		auto obj = scene->GetResourceManager().GetOpenGlLoader().GetObjectToOpenGLPostLoadingPass();
		if (obj != nullptr)
		{
			load = true;
			obj->OpenGLPostLoadingPass();
			object_loaded++;
		}
		else
		{
			load = false;
		}
		loadingScreenRenderer->Render(nullptr);
		displayManager.Update();
	}
}

void CEngine::LoadScene()
{
	if (scene == nullptr) return;
	auto start = std::chrono::high_resolution_clock::now();
	scene->Initialize();
	auto end = std::chrono::high_resolution_clock::now();
	Log("Scene loading time: " + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() / 1000.f ) + "s.");
//	std::this_thread::sleep_for(std::chrono::seconds(10));
	SetIsLoading(false);
}

void CEngine::SetIsLoading(bool is)
{
	std::lock_guard<std::mutex> lock(loadingMutex);
	isLoading = is;
}

bool CEngine::GetIsLoading()
{
	std::lock_guard<std::mutex> lock(loadingMutex);
	return isLoading;
}

void CEngine::Init()
{
	glEnable(GL_DEPTH_TEST);

	if (renderers.empty())
	{
		//Log("Renderer not set, take default renderer (FullRenderer).");
		
		auto renderer = SConfiguration::Instance().rendererType;

		if (renderer == SConfiguration::RendererType::FULL_RENDERER)
		{
			renderers.emplace_back(new FullRenderer(&projection));
		}
		else
		{
			renderers.emplace_back(new SimpleRenderer(&projection));
		}

	}
	for(auto& renderer : renderers)
		renderer->Init();
	
	auto circleTexture	= resorceManager.GetTextureLaoder().LoadTextureImmediately("../Data/GUI/circle2.png", false);
    auto bgtexture		= resorceManager.GetTextureLaoder().LoadTextureImmediately("../Data/GUI/black-knight-dark-souls.png", false, TextureType::MATERIAL, TextureFlip::Type::VERTICAL);
	loadingScreenRenderer = std::make_unique<CLoadingScreenRenderer>(bgtexture, circleTexture);
	loadingScreenRenderer->Init();
}
void CEngine::PreperaScene()
{
	isLoading = true;
	std::thread loading_thread(&CEngine::LoadScene, this);
	OpenGLLoadingPass(loading_thread);
}
