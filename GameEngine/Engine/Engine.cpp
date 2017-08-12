#include "Engine.h"
#include "Configuration.h"
#include "../Api/ApiMessages.h"
#include "../Scene/Scene.hpp"
#include "../Scene/SceneLoader.h"
#include "../Renderers/FullRenderer.h"
#include "../Renderers/SimpleRenderer.h"
#include "../Renderers/GUI/GuiRenderer.h"
#include "Logger/Log.h"
#include <fstream>

CEngine::CEngine()
{
	ReadConfigFile("Conf.xml");
	SetDisplay();
}

CEngine::~CEngine()
{
	EngineConf_SaveRequiredFiles();
}

void CEngine::ReadConfigFile(const std::string & file_name)
{
	auto& conf = EngineConf;
	EngineConf.ReadFromFile("Conf.xml");
	EngineConf_AddRequiredFile("Conf.xml");
}

void CEngine::SetDisplay()
{
	auto& conf = EngineConf;
	displayManager = CDisplayManager(conf.windowName, conf.resolution.x, conf.resolution.y, conf.fullScreen);
	displayManager.SetInput(inputManager.input);
	projection = conf.resolution;
}

void CEngine::GameLoop()
{
	ApiMessages::Type apiMessage = ApiMessages::NONE;

	while (apiMessage != ApiMessages::QUIT)
		apiMessage = MainLoop();
}

ApiMessages::Type CEngine::MainLoop()
{
	ApiMessages::Type apiMessage = ApiMessages::NONE;

	LoadObjects();
	apiMessage = PrepareFrame();

	if (inputManager.GetKey(KeyCodes::ESCAPE))
		apiMessage = ApiMessages::QUIT;

	ProccesScene();
	displayManager.Update();
	inputManager.CheckReleasedKeys();

	return apiMessage;
}

ApiMessages::Type CEngine::ProccesScene()
{
	ApiMessages::Type message = ApiMessages::NONE;
	if (scene == nullptr)
	{
		Log("CEngine::ProccesScene(): No scene set!");
		return message;
	}

	message = CheckSceneMessages();

	RenderScene();

	return message;
}

void CEngine::RenderScene()
{
	for (auto& renderer : renderers)
		Render(renderer.get());
}

void CEngine::Render(CRenderer* renderer)
{
	renderer->PrepareFrame(scene.get());
	renderer->Render(scene.get());
	renderer->EndFrame(scene.get());
}

void CEngine::LoadObjects()
{
	auto obj = scene->GetResourceManager().GetOpenGlLoader().GetObjectToOpenGLLoadingPass();
	if (obj != nullptr && !obj->isInOpenGL())
		obj->OpenGLLoadingPass();
}

ApiMessages::Type CEngine::CheckSceneMessages()
{
	switch (scene->Update())
	{
	case 1: return ApiMessages::QUIT; break;
	}

	return ApiMessages::NONE;
}

ApiMessages::Type CEngine::PrepareFrame()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(.8f, .8f, .8f, 1.f);
	return displayManager.PeekMessage();
}

void CEngine::Init()
{
	glEnable(GL_DEPTH_TEST);
	SetDefaultRenderer();
	InitRenderers();
}

void CEngine::PreperaScene()
{
	LoadScene();
}

void CEngine::SetDefaultRenderer()
{
	if (!renderers.empty())
		return;

	auto rendererType = SConfiguration::Instance().rendererType;

	if (rendererType == SConfiguration::RendererType::FULL_RENDERER)
		renderers.emplace_back(new FullRenderer(&projection));
	else
		renderers.emplace_back(new SimpleRenderer(&projection));

}

void CEngine::LoadScene()
{
	SceneLoader sceneLoader(displayManager, resorceManager);
	sceneLoader.Load(scene.get());
}

void CEngine::InitRenderers()
{
	for (auto& renderer : renderers)
		renderer->Init();
}
