#include "Engine.h"
#include "../Renderers/FullRenderer.h"
#include "../Renderers/SimpleRenderer.h"
#include "../Renderers/GUI/GuiRenderer.h"
#include "../Debug_/Log.h"
#include <fstream>

CEngine::CEngine()
    : m_DisplayManager("window_name", 1000, 600, 0)
    , m_Projection({1000, 600})
{
	m_DisplayManager.SetInput(m_InputManager.m_Input);
    ReadConfiguration("../Data/Conf.ini");
}

void CEngine::GameLoop()
{
	ApiMessages::Type m_ApiMessage = ApiMessages::NONE;

	while (m_ApiMessage != ApiMessages::QUIT)
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(.8f, .8f, .8f, 1.f);

		m_ApiMessage = m_DisplayManager.PeekMessage();

		if (m_InputManager.GetKey(KeyCodes::ESCAPE))
			m_ApiMessage = ApiMessages::QUIT;		

		if (m_Scene != nullptr)
		{
			switch (m_Scene->Update())
			{
			case 1: m_ApiMessage = ApiMessages::QUIT; break;
			}

			for (auto& renderer : m_Renderers)
			{
				renderer->PrepareFrame(m_Scene.get());
				renderer->Render(m_Scene.get());
				renderer->EndFrame(m_Scene.get());
			}						
		}
		m_DisplayManager.Update();
		m_InputManager.CheckReleasedKeys();
	}
}

void CEngine::OpenGLLoadingPass(std::thread& loading_thread)
{
	auto object_count = m_Scene->m_ObjectCount;
	auto object_loaded = 0;

	if (object_count <= 0)
		object_count = 1;

	int x = 0;
	bool  load = true;
	m_DisplayManager.GetSync() = true;
	bool post_load = false;
	while (load)
	{
		m_DisplayManager.PeekMessage();

		load = GetIsLoading();

		auto obj = m_Scene->GetResourceManager().GetOpenGlLoader().GetObjectToOpenGLLoadingPass();
		if (obj != nullptr)
		{
			load = true;
			obj->OpenGLLoadingPass();
			object_loaded++;

			std::cout << "Loading... " + std::to_string((int)((float)object_loaded / (float)object_count) *100.f) + "%" << std::endl;
		}

		m_LoadingScreenRenderer->Render(nullptr);
		m_DisplayManager.Update();
	}
	loading_thread.join();
	load = true;
	while (load)
	{
		m_DisplayManager.PeekMessage();
		auto obj = m_Scene->GetResourceManager().GetOpenGlLoader().GetObjectToOpenGLPostLoadingPass();
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
		m_LoadingScreenRenderer->Render(nullptr);
		m_DisplayManager.Update();
	}
}

void CEngine::LoadScene()
{
	if (m_Scene == nullptr) return;
	auto start = std::chrono::high_resolution_clock::now();
	m_Scene->Initialize();
	auto end = std::chrono::high_resolution_clock::now();
	Log("Scene loading time: " + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() / 1000.f ) + "s.");
//	std::this_thread::sleep_for(std::chrono::seconds(10));
	SetIsLoading(false);
}

void CEngine::SetIsLoading(bool is)
{
	std::lock_guard<std::mutex> lock(m_LoadingMutex);
	m_IsLoading = is;
}

bool CEngine::GetIsLoading()
{
	std::lock_guard<std::mutex> lock(m_LoadingMutex);
	return m_IsLoading;
}

int CEngine::ReadConfiguration(const std::string & file_name)
{
	std::ifstream file;
	file.open(file_name);
	if (!file.is_open())
	{
		Log("[Error] Cant open configuration file.");
		return -1;
	}
	std::string line;

	while (std::getline(file, line))
	{
		std::string var = line.substr(0, line.find_last_of("="));
		std::string value = line.substr(line.find_last_of("=") + 1);

        /*
        if (var.compare("Name") == 0)				m_Configuration.m_WindowName = value;
        if (var.compare("Resolution") == 0)			m_Configuration.m_WindowSize = Get::Vector2d(value);
        if (var.compare("FullScreen") == 0)			m_Configuration.m_FullScreen = Get::Boolean(value);
		if (var.compare("RefreshRate") == 0)		m_RefreshRate = Get::Int(value);
		if (var.compare("Sound") == 0)				m_IsSound = Get::Boolean(value);
		if (var.compare("SoundVolume") == 0)		m_SoundVolume = Get::Float(value);
		if (var.compare("RenderingResolution") == 0)	m_RenderingResolutionModifier = Get::Float(value);
		if (var.compare("WaterQuality") == 0)		m_WaterQuality = Get::Float(value);
		if (var.compare("WaterReflectionResolution") == 0)	m_ReflectionSize = Get::Vector2d(value);
		if (var.compare("WaterRefractionResolution") == 0)	m_RefractionSize = Get::Vector2d(value);
		if (var.compare("TextureMaxResolution") == 0)	m_MaxTextureResolution = Get::Vector2d(value);
		if (var.compare("Shadows") == 0)			m_IsShadows = Get::Boolean(value);
		if (var.compare("ShadowsDistance") == 0)	m_ShadowsDistance = Get::Float(value);
		if (var.compare("ShadowMapSize") == 0)		m_ShadowMapSize = Get::Float(value);
		if (var.compare("ViewDistance") == 0)		m_ViewDistance = Get::Float(value);
		if (var.compare("GrassViewDistance") == 0)	m_GrassViewDistance = Get::Float(value);
        if (var.compare("SimpleRender") == 0)		m_UsingSimpleRender = Get::Boolean(value); //*/

	}
	file.close();
	return 0;
}

void CEngine::Init()
{
	glEnable(GL_DEPTH_TEST);

	//if (m_Renderer == nullptr)
	//{
	//	Error("Main Renderer not set!");
	//		exit(-1);
	//}
	if (m_Renderers.empty())
	{
		Log("Renderer not set, take default renderer (FullRenderer).");
		m_Renderers.emplace_back(new FullRenderer(&m_Projection));
	}
	for(auto& renderer : m_Renderers)
		renderer->Init();
	
	auto circleTexture	= m_ResorceManager.GetTextureLaoder().LoadTextureImmediately("../Data/GUI/circle2.png");
	auto bgtexture		= m_ResorceManager.GetTextureLaoder().LoadTextureImmediately("../Data/GUI/black-knight-dark-souls.png", TextureType::MATERIAL, TextureFlip::VERTICAL);
	m_LoadingScreenRenderer = std::make_unique<CLoadingScreenRenderer>(bgtexture, circleTexture);
	m_LoadingScreenRenderer->Init();
}
void CEngine::PreperaScene()
{
	m_IsLoading = true;
	std::thread loading_thread(&CEngine::LoadScene, this);
	OpenGLLoadingPass(loading_thread);
}
