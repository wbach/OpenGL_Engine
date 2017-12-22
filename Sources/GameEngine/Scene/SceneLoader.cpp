#include "SceneLoader.h"
#include "Scene.hpp"
#include "../Display/DisplayManager.hpp"
#include "../Renderers/LodingScreenRenderer.h"
#include "Logger/Log.h"
#include "../Input/Input.h"

SceneLoader::SceneLoader(CDisplayManager & displayManager, CResourceManager & resorceManager)
	: displayManager(displayManager)
	, resorceManager(resorceManager)
	, objectLoaded(0)
	, objectCount(0)
{
	Init();
}

SceneLoader::~SceneLoader()
{
}

void SceneLoader::Load(CScene * scene)
{
	isLoading = true;
	std::thread loading_thread(&SceneLoader::LoadScene, this, scene);
	OpenGLLoadingPass(scene, loading_thread);
}

void SceneLoader::OpenGLLoadingPass(CScene* scene, std::thread & loading_thread)
{
	CheckObjectCount(scene);
	displayManager.GetSync() = true;
	LoadingLoop(scene);
	loading_thread.join();
	PostLoadingPass(scene);
}

void SceneLoader::Init()
{
	auto circleTexture = resorceManager.GetTextureLaoder().LoadTextureImmediately("GUI/circle2.png", false);
	auto bgtexture = resorceManager.GetTextureLaoder().LoadTextureImmediately("GUI/black-knight-dark-souls.png", false, TextureType::MATERIAL, TextureFlip::Type::VERTICAL);
	loadingScreenRenderer = std::make_unique<CLoadingScreenRenderer>(bgtexture, circleTexture);
	loadingScreenRenderer->Init();
}

void SceneLoader::SetIsLoading(bool is)
{
	std::lock_guard<std::mutex> lock(loadingMutex);
	isLoading = is;
}

bool SceneLoader::GetIsLoading()
{
	std::lock_guard<std::mutex> lock(loadingMutex);
	return isLoading;
}

bool SceneLoader::ProccesLoadingLoop(COpenGLObject* obj)
{
	displayManager.PeekApiMessage();

	auto load = GetIsLoading();
	if (LoadObject(obj))
		load = true;
	UpdateScreen();

	return load;
}

bool SceneLoader::LoadObject(COpenGLObject* obj)
{
	if (obj == nullptr)
		return false;

	obj->OpenGLLoadingPass();
	objectLoaded++;

	std::cout << "Loading... " + std::to_string((int) ((float) objectLoaded / (float) objectCount) *100.f) + "%" << std::endl;
	return true;
}

void SceneLoader::UpdateScreen()
{
	if (loadingScreenRenderer != nullptr)
		loadingScreenRenderer->Render(nullptr);
	displayManager.Update();
}

void SceneLoader::LoadingLoop(CScene * scene)
{
	bool load = true;
	auto& objLoader = scene->GetResourceManager().GetOpenGlLoader();

	while (load)
		load = ProccesLoadingLoop(objLoader.GetObjectToOpenGLLoadingPass());
}

void SceneLoader::CheckObjectCount(CScene * scene)
{
	objectCount = scene->objectCount;
	if (objectCount <= 0)
		objectCount = 1;
}

void SceneLoader::PostLoadingPass(CScene* scene)
{
	bool load = true;
	auto& objLoader = scene->GetResourceManager().GetOpenGlLoader();

	while (load)
		load = ProccesLoadingLoop(objLoader.GetObjectToOpenGLPostLoadingPass());

/*

	bool load = true;
	while (load)
	{
		displayManager.PeekMessage();
		auto obj = scene->GetResourceManager().GetOpenGlLoader().GetObjectToOpenGLPostLoadingPass();
		if (obj != nullptr)
		{
			load = true;
			obj->OpenGLPostLoadingPass();
			objectLoaded++;
		}
		else
		{
			load = false;
		}

		if (loadingScreenRenderer != nullptr)
			loadingScreenRenderer->Render(nullptr);
		displayManager.Update();
	}*/
}

void SceneLoader::LoadScene(CScene* scene)
{
	if (scene == nullptr)
		return;

	auto start = std::chrono::high_resolution_clock::now();
	scene->Initialize();
	auto end = std::chrono::high_resolution_clock::now();
	Log("Scene loading time: " + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() / 1000.f) + "s.");
	SetIsLoading(false);
}