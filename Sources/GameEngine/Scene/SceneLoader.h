#pragma once
#include "../Resources/ResourceManager.h"
#include "Thread.hpp"
#include "Mutex.hpp"
#include <memory>

class CScene;
class COpenGLObject;
class CDisplayManager;
class CResourceManager;
class CLoadingScreenRenderer;

class SceneLoader
{
public:
	SceneLoader(std::shared_ptr<CDisplayManager>& displayManager);
	~SceneLoader();
	bool Load(CScene* scene);

private:
	void Init();
	void LoadScene(CScene* scene);
	void SetIsLoading(bool is);
	bool GetIsLoading();
	bool ProccesLoadingLoop(COpenGLObject * obj);
	bool LoadObject(COpenGLObject * obj);
	void UpdateScreen();
	void LoadingLoop(CScene* scene);
	void CheckObjectCount(CScene* scene);
	void PostLoadingPass(CScene* scene);
	void OpenGLLoadingPass(CScene* scene, std::thread& loading_thread);

private:
	int objectCount;
	int objectLoaded;
	std::unique_ptr<CLoadingScreenRenderer> loadingScreenRenderer;

	bool isLoading;
	std::mutex loadingMutex;

	std::shared_ptr<CDisplayManager>& displayManager;
	CResourceManager resorceManager;
};
