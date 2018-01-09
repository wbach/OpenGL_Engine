#pragma once
#include <memory>
#include "Thread.hpp"
#include "Mutex.hpp"

class CScene;
class COpenGLObject;
class CDisplayManager;
class CResourceManager;
class CLoadingScreenRenderer;

class SceneLoader
{
public:
	SceneLoader(CDisplayManager& displayManager, CResourceManager& resorceManager);
	~SceneLoader();
	void Load(CScene* scene);

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

	CDisplayManager& displayManager;
	CResourceManager& resorceManager;
};
